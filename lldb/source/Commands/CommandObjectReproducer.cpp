//===-- CommandObjectReproducer.cpp -----------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "CommandObjectReproducer.h"

#include "lldb/Host/OptionParser.h"
#include "lldb/Utility/Reproducer.h"

#include "lldb/Interpreter/CommandInterpreter.h"
#include "lldb/Interpreter/CommandReturnObject.h"
#include "lldb/Interpreter/OptionArgParser.h"
#include "lldb/Interpreter/OptionGroupBoolean.h"

using namespace lldb;
using namespace llvm;
using namespace lldb_private;
using namespace lldb_private::repro;

enum ReproducerProvider {
  eReproducerProviderCommands,
  eReproducerProviderFiles,
  eReproducerProviderGDB,
  eReproducerProviderVersion,
  eReproducerProviderNone
};

static constexpr OptionEnumValueElement g_reproducer_provider_type[] = {
    {
        eReproducerProviderCommands,
        "commands",
        "Command Interpreter Commands",
    },
    {
        eReproducerProviderFiles,
        "files",
        "Files",
    },
    {
        eReproducerProviderGDB,
        "gdb",
        "GDB Remote Packets",
    },
    {
        eReproducerProviderVersion,
        "version",
        "Version",
    },
    {
        eReproducerProviderNone,
        "none",
        "None",
    },
};

static constexpr OptionEnumValues ReproducerProviderType() {
  return OptionEnumValues(g_reproducer_provider_type);
}

#define LLDB_OPTIONS_reproducer
#include "CommandOptions.inc"

class CommandObjectReproducerGenerate : public CommandObjectParsed {
public:
  CommandObjectReproducerGenerate(CommandInterpreter &interpreter)
      : CommandObjectParsed(
            interpreter, "reproducer generate",
            "Generate reproducer on disk. When the debugger is in capture "
            "mode, this command will output the reproducer to a directory on "
            "disk. In replay mode this command in a no-op.",
            nullptr) {}

  ~CommandObjectReproducerGenerate() override = default;

protected:
  bool DoExecute(Args &command, CommandReturnObject &result) override {
    if (!command.empty()) {
      result.AppendErrorWithFormat("'%s' takes no arguments",
                                   m_cmd_name.c_str());
      return false;
    }

    auto &r = Reproducer::Instance();
    if (auto generator = r.GetGenerator()) {
      generator->Keep();
    } else if (r.GetLoader()) {
      // Make this operation a NOP in replay mode.
      result.SetStatus(eReturnStatusSuccessFinishNoResult);
      return result.Succeeded();
    } else {
      result.AppendErrorWithFormat("Unable to get the reproducer generator");
      result.SetStatus(eReturnStatusFailed);
      return false;
    }

    result.GetOutputStream()
        << "Reproducer written to '" << r.GetReproducerPath() << "'\n";
    result.GetOutputStream()
        << "Please have a look at the directory to assess if you're willing to "
           "share the contained information.\n";

    result.SetStatus(eReturnStatusSuccessFinishResult);
    return result.Succeeded();
  }
};

class CommandObjectReproducerStatus : public CommandObjectParsed {
public:
  CommandObjectReproducerStatus(CommandInterpreter &interpreter)
      : CommandObjectParsed(
            interpreter, "reproducer status",
            "Show the current reproducer status. In capture mode the debugger "
            "is collecting all the information it needs to create a "
            "reproducer.  In replay mode the reproducer is replaying a "
            "reproducer. When the reproducers are off, no data is collected "
            "and no reproducer can be generated.",
            nullptr) {}

  ~CommandObjectReproducerStatus() override = default;

protected:
  bool DoExecute(Args &command, CommandReturnObject &result) override {
    if (!command.empty()) {
      result.AppendErrorWithFormat("'%s' takes no arguments",
                                   m_cmd_name.c_str());
      return false;
    }

    auto &r = Reproducer::Instance();
    if (r.GetGenerator()) {
      result.GetOutputStream() << "Reproducer is in capture mode.\n";
    } else if (r.GetLoader()) {
      result.GetOutputStream() << "Reproducer is in replay mode.\n";
    } else {
      result.GetOutputStream() << "Reproducer is off.\n";
    }

    result.SetStatus(eReturnStatusSuccessFinishResult);
    return result.Succeeded();
  }
};

static void SetError(CommandReturnObject &result, Error err) {
  result.GetErrorStream().Printf("error: %s\n",
                                 toString(std::move(err)).c_str());
  result.SetStatus(eReturnStatusFailed);
}

class CommandObjectReproducerDump : public CommandObjectParsed {
public:
  CommandObjectReproducerDump(CommandInterpreter &interpreter)
      : CommandObjectParsed(interpreter, "reproducer dump",
                            "Dump the information contained in a reproducer.",
                            nullptr) {}

  ~CommandObjectReproducerDump() override = default;

  Options *GetOptions() override { return &m_options; }

  class CommandOptions : public Options {
  public:
    CommandOptions() : Options(), file() {}

    ~CommandOptions() override = default;

    Status SetOptionValue(uint32_t option_idx, StringRef option_arg,
                          ExecutionContext *execution_context) override {
      Status error;
      const int short_option = m_getopt_table[option_idx].val;

      switch (short_option) {
      case 'f':
        file.SetFile(option_arg, FileSpec::Style::native);
        FileSystem::Instance().Resolve(file);
        break;
      case 'p':
        provider = (ReproducerProvider)OptionArgParser::ToOptionEnum(
            option_arg, GetDefinitions()[option_idx].enum_values, 0, error);
        if (!error.Success())
          error.SetErrorStringWithFormat("unrecognized value for provider '%s'",
                                         option_arg.str().c_str());
        break;
      default:
        llvm_unreachable("Unimplemented option");
      }

      return error;
    }

    void OptionParsingStarting(ExecutionContext *execution_context) override {
      file.Clear();
      provider = eReproducerProviderNone;
    }

    ArrayRef<OptionDefinition> GetDefinitions() override {
      return makeArrayRef(g_reproducer_options);
    }

    FileSpec file;
    ReproducerProvider provider = eReproducerProviderNone;
  };

protected:
  bool DoExecute(Args &command, CommandReturnObject &result) override {
    if (!command.empty()) {
      result.AppendErrorWithFormat("'%s' takes no arguments",
                                   m_cmd_name.c_str());
      return false;
    }

    // If no reproducer path is specified, use the loader currently used for
    // replay. Otherwise create a new loader just for dumping.
    llvm::Optional<Loader> loader_storage;
    Loader *loader = nullptr;
    if (!m_options.file) {
      loader = Reproducer::Instance().GetLoader();
      if (loader == nullptr) {
        result.SetError(
            "Not specifying a reproducer is only support during replay.");
        result.SetStatus(eReturnStatusSuccessFinishNoResult);
        return false;
      }
    } else {
      loader_storage.emplace(m_options.file);
      loader = &(*loader_storage);
      if (Error err = loader->LoadIndex()) {
        SetError(result, std::move(err));
        return false;
      }
    }

    // If we get here we should have a valid loader.
    assert(loader);

    switch (m_options.provider) {
    case eReproducerProviderFiles: {
      FileSpec vfs_mapping = loader->GetFile<FileProvider::Info>();

      // Read the VFS mapping.
      ErrorOr<std::unique_ptr<MemoryBuffer>> buffer =
          vfs::getRealFileSystem()->getBufferForFile(vfs_mapping.GetPath());
      if (!buffer) {
        SetError(result, errorCodeToError(buffer.getError()));
        return false;
      }

      // Initialize a VFS from the given mapping.
      IntrusiveRefCntPtr<vfs::FileSystem> vfs = vfs::getVFSFromYAML(
          std::move(buffer.get()), nullptr, vfs_mapping.GetPath());

      // Dump the VFS to a buffer.
      std::string str;
      raw_string_ostream os(str);
      static_cast<vfs::RedirectingFileSystem &>(*vfs).dump(os);
      os.flush();

      // Return the string.
      result.AppendMessage(str);
      result.SetStatus(eReturnStatusSuccessFinishResult);
      return true;
    }
    case eReproducerProviderVersion: {
      FileSpec version_file = loader->GetFile<VersionProvider::Info>();

      // Load the version info into a buffer.
      ErrorOr<std::unique_ptr<MemoryBuffer>> buffer =
          vfs::getRealFileSystem()->getBufferForFile(version_file.GetPath());
      if (!buffer) {
        SetError(result, errorCodeToError(buffer.getError()));
        return false;
      }

      // Return the version string.
      StringRef version = (*buffer)->getBuffer();
      result.AppendMessage(version.str());
      result.SetStatus(eReturnStatusSuccessFinishResult);
      return true;
    }
    case eReproducerProviderCommands: {
      // Create a new command loader.
      std::unique_ptr<repro::CommandLoader> command_loader =
          repro::CommandLoader::Create(loader);
      if (!command_loader) {
        SetError(result,
                 make_error<StringError>(llvm::inconvertibleErrorCode(),
                                         "Unable to create command loader."));
        return false;
      }

      // Iterate over the command files and dump them.
      while (true) {
        llvm::Optional<std::string> command_file =
            command_loader->GetNextFile();
        if (!command_file)
          break;

        auto command_buffer = llvm::MemoryBuffer::getFile(*command_file);
        if (auto err = command_buffer.getError()) {
          SetError(result, errorCodeToError(err));
          return false;
        }
        result.AppendMessage((*command_buffer)->getBuffer());
      }

      result.SetStatus(eReturnStatusSuccessFinishResult);
      return true;
    }
    case eReproducerProviderGDB: {
      // FIXME: Dumping the GDB remote packets means moving the
      // (de)serialization code out of the GDB-remote plugin.
      result.AppendMessage("Dumping GDB remote packets isn't implemented yet.");
      result.SetStatus(eReturnStatusSuccessFinishResult);
      return true;
    }
    case eReproducerProviderNone:
      result.SetError("No valid provider specified.");
      return false;
    }

    result.SetStatus(eReturnStatusSuccessFinishNoResult);
    return result.Succeeded();
  }

private:
  CommandOptions m_options;
};

CommandObjectReproducer::CommandObjectReproducer(
    CommandInterpreter &interpreter)
    : CommandObjectMultiword(
          interpreter, "reproducer",
          "Commands for manipulate the reproducer functionality.",
          "reproducer <subcommand> [<subcommand-options>]") {
  LoadSubCommand(
      "generate",
      CommandObjectSP(new CommandObjectReproducerGenerate(interpreter)));
  LoadSubCommand("status", CommandObjectSP(
                               new CommandObjectReproducerStatus(interpreter)));
  LoadSubCommand("dump",
                 CommandObjectSP(new CommandObjectReproducerDump(interpreter)));
}

CommandObjectReproducer::~CommandObjectReproducer() = default;
