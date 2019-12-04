//===- DeclBase.h - Base Classes for representing declarations --*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//  This file defines the Decl and DeclContext interfaces.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_AST_DECLBASE_H
#define LLVM_CLANG_AST_DECLBASE_H

#include "clang/AST/ASTDumperUtils.h"
#include "clang/AST/AttrIterator.h"
#include "clang/AST/DeclarationName.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/Specifiers.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/VersionTuple.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

namespace clang {

class ASTContext;
class ASTMutationListener;
class Attr;
class BlockDecl;
class DeclContext;
class ExternalSourceSymbolAttr;
class FunctionDecl;
class FunctionType;
class IdentifierInfo;
enum Linkage : unsigned char;
class LinkageSpecDecl;
class Module;
class NamedDecl;
class ObjCCategoryDecl;
class ObjCCategoryImplDecl;
class ObjCContainerDecl;
class ObjCImplDecl;
class ObjCImplementationDecl;
class ObjCInterfaceDecl;
class ObjCMethodDecl;
class ObjCProtocolDecl;
struct PrintingPolicy;
class RecordDecl;
class SourceManager;
class Stmt;
class StoredDeclsMap;
class TemplateDecl;
class TranslationUnitDecl;
class UsingDirectiveDecl;

/// Captures the result of checking the availability of a
/// declaration.
enum AvailabilityResult {
  AR_Available = 0,
  AR_NotYetIntroduced,
  AR_Deprecated,
  AR_Unavailable
};

/// Decl - This represents one declaration (or definition), e.g. a variable,
/// typedef, function, struct, etc.
///
/// Note: There are objects tacked on before the *beginning* of Decl
/// (and its subclasses) in its Decl::operator new(). Proper alignment
/// of all subclasses (not requiring more than the alignment of Decl) is
/// asserted in DeclBase.cpp.
class alignas(8) Decl {
public:
  /// Lists the kind of concrete classes of Decl.
  enum Kind {
#define DECL(DERIVED, BASE) DERIVED,
#define ABSTRACT_DECL(DECL)
#define DECL_RANGE(BASE, START, END) \
        first##BASE = START, last##BASE = END,
#define LAST_DECL_RANGE(BASE, START, END) \
        first##BASE = START, last##BASE = END
#include "clang/AST/DeclNodes.inc"
  };

  /// A placeholder type used to construct an empty shell of a
  /// decl-derived type that will be filled in later (e.g., by some
  /// deserialization method).
  struct EmptyShell {};

  /// IdentifierNamespace - The different namespaces in which
  /// declarations may appear.  According to C99 6.2.3, there are
  /// four namespaces, labels, tags, members and ordinary
  /// identifiers.  C++ describes lookup completely differently:
  /// certain lookups merely "ignore" certain kinds of declarations,
  /// usually based on whether the declaration is of a type, etc.
  ///
  /// These are meant as bitmasks, so that searches in
  /// C++ can look into the "tag" namespace during ordinary lookup.
  ///
  /// Decl currently provides 15 bits of IDNS bits.
  enum IdentifierNamespace {
    /// Labels, declared with 'x:' and referenced with 'goto x'.
    IDNS_Label               = 0x0001,

    /// Tags, declared with 'struct foo;' and referenced with
    /// 'struct foo'.  All tags are also types.  This is what
    /// elaborated-type-specifiers look for in C.
    /// This also contains names that conflict with tags in the
    /// same scope but that are otherwise ordinary names (non-type
    /// template parameters and indirect field declarations).
    IDNS_Tag                 = 0x0002,

    /// Types, declared with 'struct foo', typedefs, etc.
    /// This is what elaborated-type-specifiers look for in C++,
    /// but note that it's ill-formed to find a non-tag.
    IDNS_Type                = 0x0004,

    /// Members, declared with object declarations within tag
    /// definitions.  In C, these can only be found by "qualified"
    /// lookup in member expressions.  In C++, they're found by
    /// normal lookup.
    IDNS_Member              = 0x0008,

    /// Namespaces, declared with 'namespace foo {}'.
    /// Lookup for nested-name-specifiers find these.
    IDNS_Namespace           = 0x0010,

    /// Ordinary names.  In C, everything that's not a label, tag,
    /// member, or function-local extern ends up here.
    IDNS_Ordinary            = 0x0020,

    /// Objective C \@protocol.
    IDNS_ObjCProtocol        = 0x0040,

    /// This declaration is a friend function.  A friend function
    /// declaration is always in this namespace but may also be in
    /// IDNS_Ordinary if it was previously declared.
    IDNS_OrdinaryFriend      = 0x0080,

    /// This declaration is a friend class.  A friend class
    /// declaration is always in this namespace but may also be in
    /// IDNS_Tag|IDNS_Type if it was previously declared.
    IDNS_TagFriend           = 0x0100,

    /// This declaration is a using declaration.  A using declaration
    /// *introduces* a number of other declarations into the current
    /// scope, and those declarations use the IDNS of their targets,
    /// but the actual using declarations go in this namespace.
    IDNS_Using               = 0x0200,

    /// This declaration is a C++ operator declared in a non-class
    /// context.  All such operators are also in IDNS_Ordinary.
    /// C++ lexical operator lookup looks for these.
    IDNS_NonMemberOperator   = 0x0400,

    /// This declaration is a function-local extern declaration of a
    /// variable or function. This may also be IDNS_Ordinary if it
    /// has been declared outside any function. These act mostly like
    /// invisible friend declarations, but are also visible to unqualified
    /// lookup within the scope of the declaring function.
    IDNS_LocalExtern         = 0x0800,

    /// This declaration is an OpenMP user defined reduction construction.
    IDNS_OMPReduction        = 0x1000,

    /// This declaration is an OpenMP user defined mapper.
    IDNS_OMPMapper           = 0x2000,
  };

  /// ObjCDeclQualifier - 'Qualifiers' written next to the return and
  /// parameter types in method declarations.  Other than remembering
  /// them and mangling them into the method's signature string, these
  /// are ignored by the compiler; they are consumed by certain
  /// remote-messaging frameworks.
  ///
  /// in, inout, and out are mutually exclusive and apply only to
  /// method parameters.  bycopy and byref are mutually exclusive and
  /// apply only to method parameters (?).  oneway applies only to
  /// results.  All of these expect their corresponding parameter to
  /// have a particular type.  None of this is currently enforced by
  /// clang.
  ///
  /// This should be kept in sync with ObjCDeclSpec::ObjCDeclQualifier.
  enum ObjCDeclQualifier {
    OBJC_TQ_None = 0x0,
    OBJC_TQ_In = 0x1,
    OBJC_TQ_Inout = 0x2,
    OBJC_TQ_Out = 0x4,
    OBJC_TQ_Bycopy = 0x8,
    OBJC_TQ_Byref = 0x10,
    OBJC_TQ_Oneway = 0x20,

    /// The nullability qualifier is set when the nullability of the
    /// result or parameter was expressed via a context-sensitive
    /// keyword.
    OBJC_TQ_CSNullability = 0x40
  };

  /// The kind of ownership a declaration has, for visibility purposes.
  /// This enumeration is designed such that higher values represent higher
  /// levels of name hiding.
  enum class ModuleOwnershipKind : unsigned {
    /// This declaration is not owned by a module.
    Unowned,

    /// This declaration has an owning module, but is globally visible
    /// (typically because its owning module is visible and we know that
    /// modules cannot later become hidden in this compilation).
    /// After serialization and deserialization, this will be converted
    /// to VisibleWhenImported.
    Visible,

    /// This declaration has an owning module, and is visible when that
    /// module is imported.
    VisibleWhenImported,

    /// This declaration has an owning module, but is only visible to
    /// lookups that occur within that module.
    ModulePrivate
  };

protected:
  /// The next declaration within the same lexical
  /// DeclContext. These pointers form the linked list that is
  /// traversed via DeclContext's decls_begin()/decls_end().
  ///
  /// The extra two bits are used for the ModuleOwnershipKind.
  llvm::PointerIntPair<Decl *, 2, ModuleOwnershipKind> NextInContextAndBits;

private:
  friend class DeclContext;

  struct MultipleDC {
    DeclContext *SemanticDC;
    DeclContext *LexicalDC;
  };

  /// DeclCtx - Holds either a DeclContext* or a MultipleDC*.
  /// For declarations that don't contain C++ scope specifiers, it contains
  /// the DeclContext where the Decl was declared.
  /// For declarations with C++ scope specifiers, it contains a MultipleDC*
  /// with the context where it semantically belongs (SemanticDC) and the
  /// context where it was lexically declared (LexicalDC).
  /// e.g.:
  ///
  ///   namespace A {
  ///      void f(); // SemanticDC == LexicalDC == 'namespace A'
  ///   }
  ///   void A::f(); // SemanticDC == namespace 'A'
  ///                // LexicalDC == global namespace
  llvm::PointerUnion<DeclContext*, MultipleDC*> DeclCtx;

  bool isInSemaDC() const { return DeclCtx.is<DeclContext*>(); }
  bool isOutOfSemaDC() const { return DeclCtx.is<MultipleDC*>(); }

  MultipleDC *getMultipleDC() const {
    return DeclCtx.get<MultipleDC*>();
  }

  DeclContext *getSemanticDC() const {
    return DeclCtx.get<DeclContext*>();
  }

  /// Loc - The location of this decl.
  SourceLocation Loc;

  /// DeclKind - This indicates which class this is.
  unsigned DeclKind : 7;

  /// InvalidDecl - This indicates a semantic error occurred.
  unsigned InvalidDecl :  1;

  /// HasAttrs - This indicates whether the decl has attributes or not.
  unsigned HasAttrs : 1;

  /// Implicit - Whether this declaration was implicitly generated by
  /// the implementation rather than explicitly written by the user.
  unsigned Implicit : 1;

  /// Whether this declaration was "used", meaning that a definition is
  /// required.
  unsigned Used : 1;

  /// Whether this declaration was "referenced".
  /// The difference with 'Used' is whether the reference appears in a
  /// evaluated context or not, e.g. functions used in uninstantiated templates
  /// are regarded as "referenced" but not "used".
  unsigned Referenced : 1;

  /// Whether this declaration is a top-level declaration (function,
  /// global variable, etc.) that is lexically inside an objc container
  /// definition.
  unsigned TopLevelDeclInObjCContainer : 1;

  /// Whether statistic collection is enabled.
  static bool StatisticsEnabled;

protected:
  friend class ASTDeclReader;
  friend class ASTDeclWriter;
  friend class ASTNodeImporter;
  friend class ASTReader;
  friend class CXXClassMemberWrapper;
  friend class LinkageComputer;
  template<typename decl_type> friend class Redeclarable;

  /// Access - Used by C++ decls for the access specifier.
  // NOTE: VC++ treats enums as signed, avoid using the AccessSpecifier enum
  unsigned Access : 2;

  /// Whether this declaration was loaded from an AST file.
  unsigned FromASTFile : 1;

  /// IdentifierNamespace - This specifies what IDNS_* namespace this lives in.
  unsigned IdentifierNamespace : 14;

  /// If 0, we have not computed the linkage of this declaration.
  /// Otherwise, it is the linkage + 1.
  mutable unsigned CacheValidAndLinkage : 3;

  /// Allocate memory for a deserialized declaration.
  ///
  /// This routine must be used to allocate memory for any declaration that is
  /// deserialized from a module file.
  ///
  /// \param Size The size of the allocated object.
  /// \param Ctx The context in which we will allocate memory.
  /// \param ID The global ID of the deserialized declaration.
  /// \param Extra The amount of extra space to allocate after the object.
  void *operator new(std::size_t Size, const ASTContext &Ctx, unsigned ID,
                     std::size_t Extra = 0);

  /// Allocate memory for a non-deserialized declaration.
  void *operator new(std::size_t Size, const ASTContext &Ctx,
                     DeclContext *Parent, std::size_t Extra = 0);

private:
  bool AccessDeclContextSanity() const;

  /// Get the module ownership kind to use for a local lexical child of \p DC,
  /// which may be either a local or (rarely) an imported declaration.
  static ModuleOwnershipKind getModuleOwnershipKindForChildOf(DeclContext *DC) {
    if (DC) {
      auto *D = cast<Decl>(DC);
      auto MOK = D->getModuleOwnershipKind();
      if (MOK != ModuleOwnershipKind::Unowned &&
          (!D->isFromASTFile() || D->hasLocalOwningModuleStorage()))
        return MOK;
      // If D is not local and we have no local module storage, then we don't
      // need to track module ownership at all.
    }
    return ModuleOwnershipKind::Unowned;
  }

public:
  Decl() = delete;
  Decl(const Decl&) = delete;
  Decl(Decl &&) = delete;
  Decl &operator=(const Decl&) = delete;
  Decl &operator=(Decl&&) = delete;

protected:
  Decl(Kind DK, DeclContext *DC, SourceLocation L)
      : NextInContextAndBits(nullptr, getModuleOwnershipKindForChildOf(DC)),
        DeclCtx(DC), Loc(L), DeclKind(DK), InvalidDecl(false), HasAttrs(false),
        Implicit(false), Used(false), Referenced(false),
        TopLevelDeclInObjCContainer(false), Access(AS_none), FromASTFile(0),
        IdentifierNamespace(getIdentifierNamespaceForKind(DK)),
        CacheValidAndLinkage(0) {
    if (StatisticsEnabled) add(DK);
  }

  Decl(Kind DK, EmptyShell Empty)
      : DeclKind(DK), InvalidDecl(false), HasAttrs(false), Implicit(false),
        Used(false), Referenced(false), TopLevelDeclInObjCContainer(false),
        Access(AS_none), FromASTFile(0),
        IdentifierNamespace(getIdentifierNamespaceForKind(DK)),
        CacheValidAndLinkage(0) {
    if (StatisticsEnabled) add(DK);
  }

  virtual ~Decl();

  /// Update a potentially out-of-date declaration.
  void updateOutOfDate(IdentifierInfo &II) const;

  Linkage getCachedLinkage() const {
    return Linkage(CacheValidAndLinkage - 1);
  }

  void setCachedLinkage(Linkage L) const {
    CacheValidAndLinkage = L + 1;
  }

  bool hasCachedLinkage() const {
    return CacheValidAndLinkage;
  }

public:
  /// Source range that this declaration covers.
  virtual SourceRange getSourceRange() const LLVM_READONLY {
    return SourceRange(getLocation(), getLocation());
  }

  SourceLocation getBeginLoc() const LLVM_READONLY {
    return getSourceRange().getBegin();
  }

  SourceLocation getEndLoc() const LLVM_READONLY {
    return getSourceRange().getEnd();
  }

  SourceLocation getLocation() const { return Loc; }
  void setLocation(SourceLocation L) { Loc = L; }

  Kind getKind() const { return static_cast<Kind>(DeclKind); }
  const char *getDeclKindName() const;

  Decl *getNextDeclInContext() { return NextInContextAndBits.getPointer(); }
  const Decl *getNextDeclInContext() const {return NextInContextAndBits.getPointer();}

  DeclContext *getDeclContext() {
    if (isInSemaDC())
      return getSemanticDC();
    return getMultipleDC()->SemanticDC;
  }
  const DeclContext *getDeclContext() const {
    return const_cast<Decl*>(this)->getDeclContext();
  }

  /// Find the innermost non-closure ancestor of this declaration,
  /// walking up through blocks, lambdas, etc.  If that ancestor is
  /// not a code context (!isFunctionOrMethod()), returns null.
  ///
  /// A declaration may be its own non-closure context.
  Decl *getNonClosureContext();
  const Decl *getNonClosureContext() const {
    return const_cast<Decl*>(this)->getNonClosureContext();
  }

  TranslationUnitDecl *getTranslationUnitDecl();
  const TranslationUnitDecl *getTranslationUnitDecl() const {
    return const_cast<Decl*>(this)->getTranslationUnitDecl();
  }

  bool isInAnonymousNamespace() const;

  bool isInStdNamespace() const;

  ASTContext &getASTContext() const LLVM_READONLY;

  void setAccess(AccessSpecifier AS) {
    Access = AS;
    assert(AccessDeclContextSanity());
  }

  AccessSpecifier getAccess() const {
    assert(AccessDeclContextSanity());
    return AccessSpecifier(Access);
  }

  /// Retrieve the access specifier for this declaration, even though
  /// it may not yet have been properly set.
  AccessSpecifier getAccessUnsafe() const {
    return AccessSpecifier(Access);
  }

  bool hasAttrs() const { return HasAttrs; }

  void setAttrs(const AttrVec& Attrs) {
    return setAttrsImpl(Attrs, getASTContext());
  }

  AttrVec &getAttrs() {
    return const_cast<AttrVec&>(const_cast<const Decl*>(this)->getAttrs());
  }

  const AttrVec &getAttrs() const;
  void dropAttrs();
  void addAttr(Attr *A);

  using attr_iterator = AttrVec::const_iterator;
  using attr_range = llvm::iterator_range<attr_iterator>;

  attr_range attrs() const {
    return attr_range(attr_begin(), attr_end());
  }

  attr_iterator attr_begin() const {
    return hasAttrs() ? getAttrs().begin() : nullptr;
  }
  attr_iterator attr_end() const {
    return hasAttrs() ? getAttrs().end() : nullptr;
  }

  template <typename T>
  void dropAttr() {
    if (!HasAttrs) return;

    AttrVec &Vec = getAttrs();
    Vec.erase(std::remove_if(Vec.begin(), Vec.end(), isa<T, Attr*>), Vec.end());

    if (Vec.empty())
      HasAttrs = false;
  }

  template <typename T>
  llvm::iterator_range<specific_attr_iterator<T>> specific_attrs() const {
    return llvm::make_range(specific_attr_begin<T>(), specific_attr_end<T>());
  }

  template <typename T>
  specific_attr_iterator<T> specific_attr_begin() const {
    return specific_attr_iterator<T>(attr_begin());
  }

  template <typename T>
  specific_attr_iterator<T> specific_attr_end() const {
    return specific_attr_iterator<T>(attr_end());
  }

  template<typename T> T *getAttr() const {
    return hasAttrs() ? getSpecificAttr<T>(getAttrs()) : nullptr;
  }

  template<typename T> bool hasAttr() const {
    return hasAttrs() && hasSpecificAttr<T>(getAttrs());
  }

  /// getMaxAlignment - return the maximum alignment specified by attributes
  /// on this decl, 0 if there are none.
  unsigned getMaxAlignment() const;

  /// setInvalidDecl - Indicates the Decl had a semantic error. This
  /// allows for graceful error recovery.
  void setInvalidDecl(bool Invalid = true);
  bool isInvalidDecl() const { return (bool) InvalidDecl; }

  /// isImplicit - Indicates whether the declaration was implicitly
  /// generated by the implementation. If false, this declaration
  /// was written explicitly in the source code.
  bool isImplicit() const { return Implicit; }
  void setImplicit(bool I = true) { Implicit = I; }

  /// Whether *any* (re-)declaration of the entity was used, meaning that
  /// a definition is required.
  ///
  /// \param CheckUsedAttr When true, also consider the "used" attribute
  /// (in addition to the "used" bit set by \c setUsed()) when determining
  /// whether the function is used.
  bool isUsed(bool CheckUsedAttr = true) const;

  /// Set whether the declaration is used, in the sense of odr-use.
  ///
  /// This should only be used immediately after creating a declaration.
  /// It intentionally doesn't notify any listeners.
  void setIsUsed() { getCanonicalDecl()->Used = true; }

  /// Mark the declaration used, in the sense of odr-use.
  ///
  /// This notifies any mutation listeners in addition to setting a bit
  /// indicating the declaration is used.
  void markUsed(ASTContext &C);

  /// Whether any declaration of this entity was referenced.
  bool isReferenced() const;

  /// Whether this declaration was referenced. This should not be relied
  /// upon for anything other than debugging.
  bool isThisDeclarationReferenced() const { return Referenced; }

  void setReferenced(bool R = true) { Referenced = R; }

  /// Whether this declaration is a top-level declaration (function,
  /// global variable, etc.) that is lexically inside an objc container
  /// definition.
  bool isTopLevelDeclInObjCContainer() const {
    return TopLevelDeclInObjCContainer;
  }

  void setTopLevelDeclInObjCContainer(bool V = true) {
    TopLevelDeclInObjCContainer = V;
  }

  /// Looks on this and related declarations for an applicable
  /// external source symbol attribute.
  ExternalSourceSymbolAttr *getExternalSourceSymbolAttr() const;

  /// Whether this declaration was marked as being private to the
  /// module in which it was defined.
  bool isModulePrivate() const {
    return getModuleOwnershipKind() == ModuleOwnershipKind::ModulePrivate;
  }

  /// Return true if this declaration has an attribute which acts as
  /// definition of the entity, such as 'alias' or 'ifunc'.
  bool hasDefiningAttr() const;

  /// Return this declaration's defining attribute if it has one.
  const Attr *getDefiningAttr() const;

protected:
  /// Specify that this declaration was marked as being private
  /// to the module in which it was defined.
  void setModulePrivate() {
    // The module-private specifier has no effect on unowned declarations.
    // FIXME: We should track this in some way for source fidelity.
    if (getModuleOwnershipKind() == ModuleOwnershipKind::Unowned)
      return;
    setModuleOwnershipKind(ModuleOwnershipKind::ModulePrivate);
  }

  /// Set the owning module ID.
  void setOwningModuleID(unsigned ID) {
    assert(isFromASTFile() && "Only works on a deserialized declaration");
    *((unsigned*)this - 2) = ID;
  }

public:
  /// Determine the availability of the given declaration.
  ///
  /// This routine will determine the most restrictive availability of
  /// the given declaration (e.g., preferring 'unavailable' to
  /// 'deprecated').
  ///
  /// \param Message If non-NULL and the result is not \c
  /// AR_Available, will be set to a (possibly empty) message
  /// describing why the declaration has not been introduced, is
  /// deprecated, or is unavailable.
  ///
  /// \param EnclosingVersion The version to compare with. If empty, assume the
  /// deployment target version.
  ///
  /// \param RealizedPlatform If non-NULL and the availability result is found
  /// in an available attribute it will set to the platform which is written in
  /// the available attribute.
  AvailabilityResult
  getAvailability(std::string *Message = nullptr,
                  VersionTuple EnclosingVersion = VersionTuple(),
                  StringRef *RealizedPlatform = nullptr) const;

  /// Retrieve the version of the target platform in which this
  /// declaration was introduced.
  ///
  /// \returns An empty version tuple if this declaration has no 'introduced'
  /// availability attributes, or the version tuple that's specified in the
  /// attribute otherwise.
  VersionTuple getVersionIntroduced() const;

  /// Determine whether this declaration is marked 'deprecated'.
  ///
  /// \param Message If non-NULL and the declaration is deprecated,
  /// this will be set to the message describing why the declaration
  /// was deprecated (which may be empty).
  bool isDeprecated(std::string *Message = nullptr) const {
    return getAvailability(Message) == AR_Deprecated;
  }

  /// Determine whether this declaration is marked 'unavailable'.
  ///
  /// \param Message If non-NULL and the declaration is unavailable,
  /// this will be set to the message describing why the declaration
  /// was made unavailable (which may be empty).
  bool isUnavailable(std::string *Message = nullptr) const {
    return getAvailability(Message) == AR_Unavailable;
  }

  /// Determine whether this is a weak-imported symbol.
  ///
  /// Weak-imported symbols are typically marked with the
  /// 'weak_import' attribute, but may also be marked with an
  /// 'availability' attribute where we're targing a platform prior to
  /// the introduction of this feature.
  bool isWeakImported() const;

  /// Determines whether this symbol can be weak-imported,
  /// e.g., whether it would be well-formed to add the weak_import
  /// attribute.
  ///
  /// \param IsDefinition Set to \c true to indicate that this
  /// declaration cannot be weak-imported because it has a definition.
  bool canBeWeakImported(bool &IsDefinition) const;

  /// Determine whether this declaration came from an AST file (such as
  /// a precompiled header or module) rather than having been parsed.
  bool isFromASTFile() const { return FromASTFile; }

  /// Retrieve the global declaration ID associated with this
  /// declaration, which specifies where this Decl was loaded from.
  unsigned getGlobalID() const {
    if (isFromASTFile())
      return *((const unsigned*)this - 1);
    return 0;
  }

  /// Retrieve the global ID of the module that owns this particular
  /// declaration.
  unsigned getOwningModuleID() const {
    if (isFromASTFile())
      return *((const unsigned*)this - 2);
    return 0;
  }

private:
  Module *getOwningModuleSlow() const;

protected:
  bool hasLocalOwningModuleStorage() const;

public:
  /// Get the imported owning module, if this decl is from an imported
  /// (non-local) module.
  Module *getImportedOwningModule() const {
    if (!isFromASTFile() || !hasOwningModule())
      return nullptr;

    return getOwningModuleSlow();
  }

  /// Get the local owning module, if known. Returns nullptr if owner is
  /// not yet known or declaration is not from a module.
  Module *getLocalOwningModule() const {
    if (isFromASTFile() || !hasOwningModule())
      return nullptr;

    assert(hasLocalOwningModuleStorage() &&
           "owned local decl but no local module storage");
    return reinterpret_cast<Module *const *>(this)[-1];
  }
  void setLocalOwningModule(Module *M) {
    assert(!isFromASTFile() && hasOwningModule() &&
           hasLocalOwningModuleStorage() &&
           "should not have a cached owning module");
    reinterpret_cast<Module **>(this)[-1] = M;
  }

  /// Is this declaration owned by some module?
  bool hasOwningModule() const {
    return getModuleOwnershipKind() != ModuleOwnershipKind::Unowned;
  }

  /// Get the module that owns this declaration (for visibility purposes).
  Module *getOwningModule() const {
    return isFromASTFile() ? getImportedOwningModule() : getLocalOwningModule();
  }

  /// Get the module that owns this declaration for linkage purposes.
  /// There only ever is such a module under the C++ Modules TS.
  ///
  /// \param IgnoreLinkage Ignore the linkage of the entity; assume that
  /// all declarations in a global module fragment are unowned.
  Module *getOwningModuleForLinkage(bool IgnoreLinkage = false) const;

  /// Determine whether this declaration might be hidden from name
  /// lookup. Note that the declaration might be visible even if this returns
  /// \c false, if the owning module is visible within the query context.
  // FIXME: Rename this to make it clearer what it does.
  bool isHidden() const {
    return (int)getModuleOwnershipKind() > (int)ModuleOwnershipKind::Visible;
  }

  /// Set that this declaration is globally visible, even if it came from a
  /// module that is not visible.
  void setVisibleDespiteOwningModule() {
    if (isHidden())
      setModuleOwnershipKind(ModuleOwnershipKind::Visible);
  }

  /// Get the kind of module ownership for this declaration.
  ModuleOwnershipKind getModuleOwnershipKind() const {
    return NextInContextAndBits.getInt();
  }

  /// Set whether this declaration is hidden from name lookup.
  void setModuleOwnershipKind(ModuleOwnershipKind MOK) {
    assert(!(getModuleOwnershipKind() == ModuleOwnershipKind::Unowned &&
             MOK != ModuleOwnershipKind::Unowned && !isFromASTFile() &&
             !hasLocalOwningModuleStorage()) &&
           "no storage available for owning module for this declaration");
    NextInContextAndBits.setInt(MOK);
  }

  unsigned getIdentifierNamespace() const {
    return IdentifierNamespace;
  }

  bool isInIdentifierNamespace(unsigned NS) const {
    return getIdentifierNamespace() & NS;
  }

  static unsigned getIdentifierNamespaceForKind(Kind DK);

  bool hasTagIdentifierNamespace() const {
    return isTagIdentifierNamespace(getIdentifierNamespace());
  }

  static bool isTagIdentifierNamespace(unsigned NS) {
    // TagDecls have Tag and Type set and may also have TagFriend.
    return (NS & ~IDNS_TagFriend) == (IDNS_Tag | IDNS_Type);
  }

  /// getLexicalDeclContext - The declaration context where this Decl was
  /// lexically declared (LexicalDC). May be different from
  /// getDeclContext() (SemanticDC).
  /// e.g.:
  ///
  ///   namespace A {
  ///      void f(); // SemanticDC == LexicalDC == 'namespace A'
  ///   }
  ///   void A::f(); // SemanticDC == namespace 'A'
  ///                // LexicalDC == global namespace
  DeclContext *getLexicalDeclContext() {
    if (isInSemaDC())
      return getSemanticDC();
    return getMultipleDC()->LexicalDC;
  }
  const DeclContext *getLexicalDeclContext() const {
    return const_cast<Decl*>(this)->getLexicalDeclContext();
  }

  /// Determine whether this declaration is declared out of line (outside its
  /// semantic context).
  virtual bool isOutOfLine() const;

  /// setDeclContext - Set both the semantic and lexical DeclContext
  /// to DC.
  void setDeclContext(DeclContext *DC);

  void setLexicalDeclContext(DeclContext *DC);

  /// Determine whether this declaration is a templated entity (whether it is
  // within the scope of a template parameter).
  bool isTemplated() const;

  /// isDefinedOutsideFunctionOrMethod - This predicate returns true if this
  /// scoped decl is defined outside the current function or method.  This is
  /// roughly global variables and functions, but also handles enums (which
  /// could be defined inside or outside a function etc).
  bool isDefinedOutsideFunctionOrMethod() const {
    return getParentFunctionOrMethod() == nullptr;
  }

  /// Returns true if this declaration lexically is inside a function.
  /// It recognizes non-defining declarations as well as members of local
  /// classes:
  /// \code
  ///     void foo() { void bar(); }
  ///     void foo2() { class ABC { void bar(); }; }
  /// \endcode
  bool isLexicallyWithinFunctionOrMethod() const;

  /// If this decl is defined inside a function/method/block it returns
  /// the corresponding DeclContext, otherwise it returns null.
  const DeclContext *getParentFunctionOrMethod() const;
  DeclContext *getParentFunctionOrMethod() {
    return const_cast<DeclContext*>(
                    const_cast<const Decl*>(this)->getParentFunctionOrMethod());
  }

  /// Retrieves the "canonical" declaration of the given declaration.
  virtual Decl *getCanonicalDecl() { return this; }
  const Decl *getCanonicalDecl() const {
    return const_cast<Decl*>(this)->getCanonicalDecl();
  }

  /// Whether this particular Decl is a canonical one.
  bool isCanonicalDecl() const { return getCanonicalDecl() == this; }

protected:
  /// Returns the next redeclaration or itself if this is the only decl.
  ///
  /// Decl subclasses that can be redeclared should override this method so that
  /// Decl::redecl_iterator can iterate over them.
  virtual Decl *getNextRedeclarationImpl() { return this; }

  /// Implementation of getPreviousDecl(), to be overridden by any
  /// subclass that has a redeclaration chain.
  virtual Decl *getPreviousDeclImpl() { return nullptr; }

  /// Implementation of getMostRecentDecl(), to be overridden by any
  /// subclass that has a redeclaration chain.
  virtual Decl *getMostRecentDeclImpl() { return this; }

public:
  /// Iterates through all the redeclarations of the same decl.
  class redecl_iterator {
    /// Current - The current declaration.
    Decl *Current = nullptr;
    Decl *Starter;

  public:
    using value_type = Decl *;
    using reference = const value_type &;
    using pointer = const value_type *;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;

    redecl_iterator() = default;
    explicit redecl_iterator(Decl *C) : Current(C), Starter(C) {}

    reference operator*() const { return Current; }
    value_type operator->() const { return Current; }

    redecl_iterator& operator++() {
      assert(Current && "Advancing while iterator has reached end");
      // Get either previous decl or latest decl.
      Decl *Next = Current->getNextRedeclarationImpl();
      assert(Next && "Should return next redeclaration or itself, never null!");
      Current = (Next != Starter) ? Next : nullptr;
      return *this;
    }

    redecl_iterator operator++(int) {
      redecl_iterator tmp(*this);
      ++(*this);
      return tmp;
    }

    friend bool operator==(redecl_iterator x, redecl_iterator y) {
      return x.Current == y.Current;
    }

    friend bool operator!=(redecl_iterator x, redecl_iterator y) {
      return x.Current != y.Current;
    }
  };

  using redecl_range = llvm::iterator_range<redecl_iterator>;

  /// Returns an iterator range for all the redeclarations of the same
  /// decl. It will iterate at least once (when this decl is the only one).
  redecl_range redecls() const {
    return redecl_range(redecls_begin(), redecls_end());
  }

  redecl_iterator redecls_begin() const {
    return redecl_iterator(const_cast<Decl *>(this));
  }

  redecl_iterator redecls_end() const { return redecl_iterator(); }

  /// Retrieve the previous declaration that declares the same entity
  /// as this declaration, or NULL if there is no previous declaration.
  Decl *getPreviousDecl() { return getPreviousDeclImpl(); }

  /// Retrieve the previous declaration that declares the same entity
  /// as this declaration, or NULL if there is no previous declaration.
  const Decl *getPreviousDecl() const {
    return const_cast<Decl *>(this)->getPreviousDeclImpl();
  }

  /// True if this is the first declaration in its redeclaration chain.
  bool isFirstDecl() const {
    return getPreviousDecl() == nullptr;
  }

  /// Retrieve the most recent declaration that declares the same entity
  /// as this declaration (which may be this declaration).
  Decl *getMostRecentDecl() { return getMostRecentDeclImpl(); }

  /// Retrieve the most recent declaration that declares the same entity
  /// as this declaration (which may be this declaration).
  const Decl *getMostRecentDecl() const {
    return const_cast<Decl *>(this)->getMostRecentDeclImpl();
  }

  /// getBody - If this Decl represents a declaration for a body of code,
  ///  such as a function or method definition, this method returns the
  ///  top-level Stmt* of that body.  Otherwise this method returns null.
  virtual Stmt* getBody() const { return nullptr; }

  /// Returns true if this \c Decl represents a declaration for a body of
  /// code, such as a function or method definition.
  /// Note that \c hasBody can also return true if any redeclaration of this
  /// \c Decl represents a declaration for a body of code.
  virtual bool hasBody() const { return getBody() != nullptr; }

  /// getBodyRBrace - Gets the right brace of the body, if a body exists.
  /// This works whether the body is a CompoundStmt or a CXXTryStmt.
  SourceLocation getBodyRBrace() const;

  // global temp stats (until we have a per-module visitor)
  static void add(Kind k);
  static void EnableStatistics();
  static void PrintStats();

  /// isTemplateParameter - Determines whether this declaration is a
  /// template parameter.
  bool isTemplateParameter() const;

  /// isTemplateParameter - Determines whether this declaration is a
  /// template parameter pack.
  bool isTemplateParameterPack() const;

  /// Whether this declaration is a parameter pack.
  bool isParameterPack() const;

  /// returns true if this declaration is a template
  bool isTemplateDecl() const;

  /// Whether this declaration is a function or function template.
  bool isFunctionOrFunctionTemplate() const {
    return (DeclKind >= Decl::firstFunction &&
            DeclKind <= Decl::lastFunction) ||
           DeclKind == FunctionTemplate;
  }

  /// If this is a declaration that describes some template, this
  /// method returns that template declaration.
  TemplateDecl *getDescribedTemplate() const;

  /// Returns the function itself, or the templated function if this is a
  /// function template.
  FunctionDecl *getAsFunction() LLVM_READONLY;

  const FunctionDecl *getAsFunction() const {
    return const_cast<Decl *>(this)->getAsFunction();
  }

  /// Changes the namespace of this declaration to reflect that it's
  /// a function-local extern declaration.
  ///
  /// These declarations appear in the lexical context of the extern
  /// declaration, but in the semantic context of the enclosing namespace
  /// scope.
  void setLocalExternDecl() {
    Decl *Prev = getPreviousDecl();
    IdentifierNamespace &= ~IDNS_Ordinary;

    // It's OK for the declaration to still have the "invisible friend" flag or
    // the "conflicts with tag declarations in this scope" flag for the outer
    // scope.
    assert((IdentifierNamespace & ~(IDNS_OrdinaryFriend | IDNS_Tag)) == 0 &&
           "namespace is not ordinary");

    IdentifierNamespace |= IDNS_LocalExtern;
    if (Prev && Prev->getIdentifierNamespace() & IDNS_Ordinary)
      IdentifierNamespace |= IDNS_Ordinary;
  }

  /// Determine whether this is a block-scope declaration with linkage.
  /// This will either be a local variable declaration declared 'extern', or a
  /// local function declaration.
  bool isLocalExternDecl() {
    return IdentifierNamespace & IDNS_LocalExtern;
  }

  /// Changes the namespace of this declaration to reflect that it's
  /// the object of a friend declaration.
  ///
  /// These declarations appear in the lexical context of the friending
  /// class, but in the semantic context of the actual entity.  This property
  /// applies only to a specific decl object;  other redeclarations of the
  /// same entity may not (and probably don't) share this property.
  void setObjectOfFriendDecl(bool PerformFriendInjection = false) {
    unsigned OldNS = IdentifierNamespace;
    assert((OldNS & (IDNS_Tag | IDNS_Ordinary |
                     IDNS_TagFriend | IDNS_OrdinaryFriend |
                     IDNS_LocalExtern | IDNS_NonMemberOperator)) &&
           "namespace includes neither ordinary nor tag");
    assert(!(OldNS & ~(IDNS_Tag | IDNS_Ordinary | IDNS_Type |
                       IDNS_TagFriend | IDNS_OrdinaryFriend |
                       IDNS_LocalExtern | IDNS_NonMemberOperator)) &&
           "namespace includes other than ordinary or tag");

    Decl *Prev = getPreviousDecl();
    IdentifierNamespace &= ~(IDNS_Ordinary | IDNS_Tag | IDNS_Type);

    if (OldNS & (IDNS_Tag | IDNS_TagFriend)) {
      IdentifierNamespace |= IDNS_TagFriend;
      if (PerformFriendInjection ||
          (Prev && Prev->getIdentifierNamespace() & IDNS_Tag))
        IdentifierNamespace |= IDNS_Tag | IDNS_Type;
    }

    if (OldNS & (IDNS_Ordinary | IDNS_OrdinaryFriend |
                 IDNS_LocalExtern | IDNS_NonMemberOperator)) {
      IdentifierNamespace |= IDNS_OrdinaryFriend;
      if (PerformFriendInjection ||
          (Prev && Prev->getIdentifierNamespace() & IDNS_Ordinary))
        IdentifierNamespace |= IDNS_Ordinary;
    }
  }

  enum FriendObjectKind {
    FOK_None,      ///< Not a friend object.
    FOK_Declared,  ///< A friend of a previously-declared entity.
    FOK_Undeclared ///< A friend of a previously-undeclared entity.
  };

  /// Determines whether this declaration is the object of a
  /// friend declaration and, if so, what kind.
  ///
  /// There is currently no direct way to find the associated FriendDecl.
  FriendObjectKind getFriendObjectKind() const {
    unsigned mask =
        (IdentifierNamespace & (IDNS_TagFriend | IDNS_OrdinaryFriend));
    if (!mask) return FOK_None;
    return (IdentifierNamespace & (IDNS_Tag | IDNS_Ordinary) ? FOK_Declared
                                                             : FOK_Undeclared);
  }

  /// Specifies that this declaration is a C++ overloaded non-member.
  void setNonMemberOperator() {
    assert(getKind() == Function || getKind() == FunctionTemplate);
    assert((IdentifierNamespace & IDNS_Ordinary) &&
           "visible non-member operators should be in ordinary namespace");
    IdentifierNamespace |= IDNS_NonMemberOperator;
  }

  static bool classofKind(Kind K) { return true; }
  static DeclContext *castToDeclContext(const Decl *);
  static Decl *castFromDeclContext(const DeclContext *);

  void print(raw_ostream &Out, unsigned Indentation = 0,
             bool PrintInstantiation = false) const;
  void print(raw_ostream &Out, const PrintingPolicy &Policy,
             unsigned Indentation = 0, bool PrintInstantiation = false) const;
  static void printGroup(Decl** Begin, unsigned NumDecls,
                         raw_ostream &Out, const PrintingPolicy &Policy,
                         unsigned Indentation = 0);

  // Debuggers don't usually respect default arguments.
  void dump() const;

  // Same as dump(), but forces color printing.
  void dumpColor() const;

  void dump(raw_ostream &Out, bool Deserialize = false,
            ASTDumpOutputFormat OutputFormat = ADOF_Default) const;

  /// \return Unique reproducible object identifier
  int64_t getID() const;

  /// Looks through the Decl's underlying type to extract a FunctionType
  /// when possible. Will return null if the type underlying the Decl does not
  /// have a FunctionType.
  const FunctionType *getFunctionType(bool BlocksToo = true) const;

private:
  void setAttrsImpl(const AttrVec& Attrs, ASTContext &Ctx);
  void setDeclContextsImpl(DeclContext *SemaDC, DeclContext *LexicalDC,
                           ASTContext &Ctx);

protected:
  ASTMutationListener *getASTMutationListener() const;
};

/// Determine whether two declarations declare the same entity.
inline bool declaresSameEntity(const Decl *D1, const Decl *D2) {
  if (!D1 || !D2)
    return false;

  if (D1 == D2)
    return true;

  return D1->getCanonicalDecl() == D2->getCanonicalDecl();
}

/// PrettyStackTraceDecl - If a crash occurs, indicate that it happened when
/// doing something to a specific decl.
class PrettyStackTraceDecl : public llvm::PrettyStackTraceEntry {
  const Decl *TheDecl;
  SourceLocation Loc;
  SourceManager &SM;
  const char *Message;

public:
  PrettyStackTraceDecl(const Decl *theDecl, SourceLocation L,
                       SourceManager &sm, const char *Msg)
      : TheDecl(theDecl), Loc(L), SM(sm), Message(Msg) {}

  void print(raw_ostream &OS) const override;
};

/// The results of name lookup within a DeclContext. This is either a
/// single result (with no stable storage) or a collection of results (with
/// stable storage provided by the lookup table).
class DeclContextLookupResult {
  using ResultTy = ArrayRef<NamedDecl *>;

  ResultTy Result;

  // If there is only one lookup result, it would be invalidated by
  // reallocations of the name table, so store it separately.
  NamedDecl *Single = nullptr;

  static NamedDecl *const SingleElementDummyList;

public:
  DeclContextLookupResult() = default;
  DeclContextLookupResult(ArrayRef<NamedDecl *> Result)
      : Result(Result) {}
  DeclContextLookupResult(NamedDecl *Single)
      : Result(SingleElementDummyList), Single(Single) {}

  class iterator;

  using IteratorBase =
      llvm::iterator_adaptor_base<iterator, ResultTy::iterator,
                                  std::random_access_iterator_tag,
                                  NamedDecl *const>;

  class iterator : public IteratorBase {
    value_type SingleElement;

  public:
    explicit iterator(pointer Pos, value_type Single = nullptr)
        : IteratorBase(Pos), SingleElement(Single) {}

    reference operator*() const {
      return SingleElement ? SingleElement : IteratorBase::operator*();
    }
  };

  using const_iterator = iterator;
  using pointer = iterator::pointer;
  using reference = iterator::reference;

  iterator begin() const { return iterator(Result.begin(), Single); }
  iterator end() const { return iterator(Result.end(), Single); }

  bool empty() const { return Result.empty(); }
  pointer data() const { return Single ? &Single : Result.data(); }
  size_t size() const { return Single ? 1 : Result.size(); }
  reference front() const { return Single ? Single : Result.front(); }
  reference back() const { return Single ? Single : Result.back(); }
  reference operator[](size_t N) const { return Single ? Single : Result[N]; }

  // FIXME: Remove this from the interface
  DeclContextLookupResult slice(size_t N) const {
    DeclContextLookupResult Sliced = Result.slice(N);
    Sliced.Single = Single;
    return Sliced;
  }
};

/// DeclContext - This is used only as base class of specific decl types that
/// can act as declaration contexts. These decls are (only the top classes
/// that directly derive from DeclContext are mentioned, not their subclasses):
///
///   TranslationUnitDecl
///   ExternCContext
///   NamespaceDecl
///   TagDecl
///   OMPDeclareReductionDecl
///   OMPDeclareMapperDecl
///   FunctionDecl
///   ObjCMethodDecl
///   ObjCContainerDecl
///   LinkageSpecDecl
///   ExportDecl
///   BlockDecl
///   CapturedDecl
class DeclContext {
  /// For makeDeclVisibleInContextImpl
  friend class ASTDeclReader;
  /// For reconcileExternalVisibleStorage, CreateStoredDeclsMap,
  /// hasNeedToReconcileExternalVisibleStorage
  friend class ExternalASTSource;
  /// For CreateStoredDeclsMap
  friend class DependentDiagnostic;
  /// For hasNeedToReconcileExternalVisibleStorage,
  /// hasLazyLocalLexicalLookups, hasLazyExternalLexicalLookups
  friend class ASTWriter;

  // We use uint64_t in the bit-fields below since some bit-fields
  // cross the unsigned boundary and this breaks the packing.

  /// Stores the bits used by DeclContext.
  /// If modified NumDeclContextBit, the ctor of DeclContext and the accessor
  /// methods in DeclContext should be updated appropriately.
  class DeclContextBitfields {
    friend class DeclContext;
    /// DeclKind - This indicates which class this is.
    uint64_t DeclKind : 7;

    /// Whether this declaration context also has some external
    /// storage that contains additional declarations that are lexically
    /// part of this context.
    mutable uint64_t ExternalLexicalStorage : 1;

    /// Whether this declaration context also has some external
    /// storage that contains additional declarations that are visible
    /// in this context.
    mutable uint64_t ExternalVisibleStorage : 1;

    /// Whether this declaration context has had externally visible
    /// storage added since the last lookup. In this case, \c LookupPtr's
    /// invariant may not hold and needs to be fixed before we perform
    /// another lookup.
    mutable uint64_t NeedToReconcileExternalVisibleStorage : 1;

    /// If \c true, this context may have local lexical declarations
    /// that are missing from the lookup table.
    mutable uint64_t HasLazyLocalLexicalLookups : 1;

    /// If \c true, the external source may have lexical declarations
    /// that are missing from the lookup table.
    mutable uint64_t HasLazyExternalLexicalLookups : 1;

    /// If \c true, lookups should only return identifier from
    /// DeclContext scope (for example TranslationUnit). Used in
    /// LookupQualifiedName()
    mutable uint64_t UseQualifiedLookup : 1;
  };

  /// Number of bits in DeclContextBitfields.
  enum { NumDeclContextBits = 13 };

  /// Stores the bits used by TagDecl.
  /// If modified NumTagDeclBits and the accessor
  /// methods in TagDecl should be updated appropriately.
  class TagDeclBitfields {
    friend class TagDecl;
    /// For the bits in DeclContextBitfields
    uint64_t : NumDeclContextBits;

    /// The TagKind enum.
    uint64_t TagDeclKind : 3;

    /// True if this is a definition ("struct foo {};"), false if it is a
    /// declaration ("struct foo;").  It is not considered a definition
    /// until the definition has been fully processed.
    uint64_t IsCompleteDefinition : 1;

    /// True if this is currently being defined.
    uint64_t IsBeingDefined : 1;

    /// True if this tag declaration is "embedded" (i.e., defined or declared
    /// for the very first time) in the syntax of a declarator.
    uint64_t IsEmbeddedInDeclarator : 1;

    /// True if this tag is free standing, e.g. "struct foo;".
    uint64_t IsFreeStanding : 1;

    /// Indicates whether it is possible for declarations of this kind
    /// to have an out-of-date definition.
    ///
    /// This option is only enabled when modules are enabled.
    uint64_t MayHaveOutOfDateDef : 1;

    /// Has the full definition of this type been required by a use somewhere in
    /// the TU.
    uint64_t IsCompleteDefinitionRequired : 1;
  };

  /// Number of non-inherited bits in TagDeclBitfields.
  enum { NumTagDeclBits = 9 };

  /// Stores the bits used by EnumDecl.
  /// If modified NumEnumDeclBit and the accessor
  /// methods in EnumDecl should be updated appropriately.
  class EnumDeclBitfields {
    friend class EnumDecl;
    /// For the bits in DeclContextBitfields.
    uint64_t : NumDeclContextBits;
    /// For the bits in TagDeclBitfields.
    uint64_t : NumTagDeclBits;

    /// Width in bits required to store all the non-negative
    /// enumerators of this enum.
    uint64_t NumPositiveBits : 8;

    /// Width in bits required to store all the negative
    /// enumerators of this enum.
    uint64_t NumNegativeBits : 8;

    /// True if this tag declaration is a scoped enumeration. Only
    /// possible in C++11 mode.
    uint64_t IsScoped : 1;

    /// If this tag declaration is a scoped enum,
    /// then this is true if the scoped enum was declared using the class
    /// tag, false if it was declared with the struct tag. No meaning is
    /// associated if this tag declaration is not a scoped enum.
    uint64_t IsScopedUsingClassTag : 1;

    /// True if this is an enumeration with fixed underlying type. Only
    /// possible in C++11, Microsoft extensions, or Objective C mode.
    uint64_t IsFixed : 1;

    /// True if a valid hash is stored in ODRHash.
    uint64_t HasODRHash : 1;
  };

  /// Number of non-inherited bits in EnumDeclBitfields.
  enum { NumEnumDeclBits = 20 };

  /// Stores the bits used by RecordDecl.
  /// If modified NumRecordDeclBits and the accessor
  /// methods in RecordDecl should be updated appropriately.
  class RecordDeclBitfields {
    friend class RecordDecl;
    /// For the bits in DeclContextBitfields.
    uint64_t : NumDeclContextBits;
    /// For the bits in TagDeclBitfields.
    uint64_t : NumTagDeclBits;

    /// This is true if this struct ends with a flexible
    /// array member (e.g. int X[]) or if this union contains a struct that does.
    /// If so, this cannot be contained in arrays or other structs as a member.
    uint64_t HasFlexibleArrayMember : 1;

    /// Whether this is the type of an anonymous struct or union.
    uint64_t AnonymousStructOrUnion : 1;

    /// This is true if this struct has at least one member
    /// containing an Objective-C object pointer type.
    uint64_t HasObjectMember : 1;

    /// This is true if struct has at least one member of
    /// 'volatile' type.
    uint64_t HasVolatileMember : 1;

    /// Whether the field declarations of this record have been loaded
    /// from external storage. To avoid unnecessary deserialization of
    /// methods/nested types we allow deserialization of just the fields
    /// when needed.
    mutable uint64_t LoadedFieldsFromExternalStorage : 1;

    /// Basic properties of non-trivial C structs.
    uint64_t NonTrivialToPrimitiveDefaultInitialize : 1;
    uint64_t NonTrivialToPrimitiveCopy : 1;
    uint64_t NonTrivialToPrimitiveDestroy : 1;

    /// The following bits indicate whether this is or contains a C union that
    /// is non-trivial to default-initialize, destruct, or copy. These bits
    /// imply the associated basic non-triviality predicates declared above.
    uint64_t HasNonTrivialToPrimitiveDefaultInitializeCUnion : 1;
    uint64_t HasNonTrivialToPrimitiveDestructCUnion : 1;
    uint64_t HasNonTrivialToPrimitiveCopyCUnion : 1;

    /// Indicates whether this struct is destroyed in the callee.
    uint64_t ParamDestroyedInCallee : 1;

    /// Represents the way this type is passed to a function.
    uint64_t ArgPassingRestrictions : 2;
  };

  /// Number of non-inherited bits in RecordDeclBitfields.
  enum { NumRecordDeclBits = 14 };

  /// Stores the bits used by OMPDeclareReductionDecl.
  /// If modified NumOMPDeclareReductionDeclBits and the accessor
  /// methods in OMPDeclareReductionDecl should be updated appropriately.
  class OMPDeclareReductionDeclBitfields {
    friend class OMPDeclareReductionDecl;
    /// For the bits in DeclContextBitfields
    uint64_t : NumDeclContextBits;

    /// Kind of initializer,
    /// function call or omp_priv<init_expr> initializtion.
    uint64_t InitializerKind : 2;
  };

  /// Number of non-inherited bits in OMPDeclareReductionDeclBitfields.
  enum { NumOMPDeclareReductionDeclBits = 2 };

  /// Stores the bits used by FunctionDecl.
  /// If modified NumFunctionDeclBits and the accessor
  /// methods in FunctionDecl and CXXDeductionGuideDecl
  /// (for IsCopyDeductionCandidate) should be updated appropriately.
  class FunctionDeclBitfields {
    friend class FunctionDecl;
    /// For IsCopyDeductionCandidate
    friend class CXXDeductionGuideDecl;
    /// For the bits in DeclContextBitfields.
    uint64_t : NumDeclContextBits;

    uint64_t SClass : 3;
    uint64_t IsInline : 1;
    uint64_t IsInlineSpecified : 1;

    uint64_t IsVirtualAsWritten : 1;
    uint64_t IsPure : 1;
    uint64_t HasInheritedPrototype : 1;
    uint64_t HasWrittenPrototype : 1;
    uint64_t IsDeleted : 1;
    /// Used by CXXMethodDecl
    uint64_t IsTrivial : 1;

    /// This flag indicates whether this function is trivial for the purpose of
    /// calls. This is meaningful only when this function is a copy/move
    /// constructor or a destructor.
    uint64_t IsTrivialForCall : 1;

    /// Used by CXXMethodDecl
    uint64_t IsDefaulted : 1;
    /// Used by CXXMethodDecl
    uint64_t IsExplicitlyDefaulted : 1;
    uint64_t HasImplicitReturnZero : 1;
    uint64_t IsLateTemplateParsed : 1;

    /// Kind of contexpr specifier as defined by ConstexprSpecKind.
    uint64_t ConstexprKind : 2;
    uint64_t InstantiationIsPending : 1;

    /// Indicates if the function uses __try.
    uint64_t UsesSEHTry : 1;

    /// Indicates if the function was a definition
    /// but its body was skipped.
    uint64_t HasSkippedBody : 1;

    /// Indicates if the function declaration will
    /// have a body, once we're done parsing it.
    uint64_t WillHaveBody : 1;

    /// Indicates that this function is a multiversioned
    /// function using attribute 'target'.
    uint64_t IsMultiVersion : 1;

    /// [C++17] Only used by CXXDeductionGuideDecl. Indicates that
    /// the Deduction Guide is the implicitly generated 'copy
    /// deduction candidate' (is used during overload resolution).
    uint64_t IsCopyDeductionCandidate : 1;

    /// Store the ODRHash after first calculation.
    uint64_t HasODRHash : 1;

    /// Indicates if the function uses Floating Point Constrained Intrinsics
    uint64_t UsesFPIntrin : 1;
  };

  /// Number of non-inherited bits in FunctionDeclBitfields.
  enum { NumFunctionDeclBits = 26 };

  /// Stores the bits used by CXXConstructorDecl. If modified
  /// NumCXXConstructorDeclBits and the accessor
  /// methods in CXXConstructorDecl should be updated appropriately.
  class CXXConstructorDeclBitfields {
    friend class CXXConstructorDecl;
    /// For the bits in DeclContextBitfields.
    uint64_t : NumDeclContextBits;
    /// For the bits in FunctionDeclBitfields.
    uint64_t : NumFunctionDeclBits;

    /// 24 bits to fit in the remaining available space.
    /// Note that this makes CXXConstructorDeclBitfields take
    /// exactly 64 bits and thus the width of NumCtorInitializers
    /// will need to be shrunk if some bit is added to NumDeclContextBitfields,
    /// NumFunctionDeclBitfields or CXXConstructorDeclBitfields.
    uint64_t NumCtorInitializers : 22;
    uint64_t IsInheritingConstructor : 1;

    /// Whether this constructor has a trail-allocated explicit specifier.
    uint64_t HasTrailingExplicitSpecifier : 1;
    /// If this constructor does't have a trail-allocated explicit specifier.
    /// Whether this constructor is explicit specified.
    uint64_t IsSimpleExplicit : 1;
  };

  /// Number of non-inherited bits in CXXConstructorDeclBitfields.
  enum {
    NumCXXConstructorDeclBits = 64 - NumDeclContextBits - NumFunctionDeclBits
  };

  /// Stores the bits used by ObjCMethodDecl.
  /// If modified NumObjCMethodDeclBits and the accessor
  /// methods in ObjCMethodDecl should be updated appropriately.
  class ObjCMethodDeclBitfields {
    friend class ObjCMethodDecl;

    /// For the bits in DeclContextBitfields.
    uint64_t : NumDeclContextBits;

    /// The conventional meaning of this method; an ObjCMethodFamily.
    /// This is not serialized; instead, it is computed on demand and
    /// cached.
    mutable uint64_t Family : ObjCMethodFamilyBitWidth;

    /// instance (true) or class (false) method.
    uint64_t IsInstance : 1;
    uint64_t IsVariadic : 1;

    /// True if this method is the getter or setter for an explicit property.
    uint64_t IsPropertyAccessor : 1;

    /// True if this method is a synthesized property accessor stub.
    uint64_t IsSynthesizedAccessorStub : 1;

    /// Method has a definition.
    uint64_t IsDefined : 1;

    /// Method redeclaration in the same interface.
    uint64_t IsRedeclaration : 1;

    /// Is redeclared in the same interface.
    mutable uint64_t HasRedeclaration : 1;

    /// \@required/\@optional
    uint64_t DeclImplementation : 2;

    /// in, inout, etc.
    uint64_t objcDeclQualifier : 7;

    /// Indicates whether this method has a related result type.
    uint64_t RelatedResultType : 1;

    /// Whether the locations of the selector identifiers are in a
    /// "standard" position, a enum SelectorLocationsKind.
    uint64_t SelLocsKind : 2;

    /// Whether this method overrides any other in the class hierarchy.
    ///
    /// A method is said to override any method in the class's
    /// base classes, its protocols, or its categories' protocols, that has
    /// the same selector and is of the same kind (class or instance).
    /// A method in an implementation is not considered as overriding the same
    /// method in the interface or its categories.
    uint64_t IsOverriding : 1;

    /// Indicates if the method was a definition but its body was skipped.
    uint64_t HasSkippedBody : 1;
  };

  /// Number of non-inherited bits in ObjCMethodDeclBitfields.
  enum { NumObjCMethodDeclBits = 24 };

  /// Stores the bits used by ObjCContainerDecl.
  /// If modified NumObjCContainerDeclBits and the accessor
  /// methods in ObjCContainerDecl should be updated appropriately.
  class ObjCContainerDeclBitfields {
    friend class ObjCContainerDecl;
    /// For the bits in DeclContextBitfields
    uint32_t : NumDeclContextBits;

    // Not a bitfield but this saves space.
    // Note that ObjCContainerDeclBitfields is full.
    SourceLocation AtStart;
  };

  /// Number of non-inherited bits in ObjCContainerDeclBitfields.
  /// Note that here we rely on the fact that SourceLocation is 32 bits
  /// wide. We check this with the static_assert in the ctor of DeclContext.
  enum { NumObjCContainerDeclBits = 64 - NumDeclContextBits };

  /// Stores the bits used by LinkageSpecDecl.
  /// If modified NumLinkageSpecDeclBits and the accessor
  /// methods in LinkageSpecDecl should be updated appropriately.
  class LinkageSpecDeclBitfields {
    friend class LinkageSpecDecl;
    /// For the bits in DeclContextBitfields.
    uint64_t : NumDeclContextBits;

    /// The language for this linkage specification with values
    /// in the enum LinkageSpecDecl::LanguageIDs.
    uint64_t Language : 3;

    /// True if this linkage spec has braces.
    /// This is needed so that hasBraces() returns the correct result while the
    /// linkage spec body is being parsed.  Once RBraceLoc has been set this is
    /// not used, so it doesn't need to be serialized.
    uint64_t HasBraces : 1;
  };

  /// Number of non-inherited bits in LinkageSpecDeclBitfields.
  enum { NumLinkageSpecDeclBits = 4 };

  /// Stores the bits used by BlockDecl.
  /// If modified NumBlockDeclBits and the accessor
  /// methods in BlockDecl should be updated appropriately.
  class BlockDeclBitfields {
    friend class BlockDecl;
    /// For the bits in DeclContextBitfields.
    uint64_t : NumDeclContextBits;

    uint64_t IsVariadic : 1;
    uint64_t CapturesCXXThis : 1;
    uint64_t BlockMissingReturnType : 1;
    uint64_t IsConversionFromLambda : 1;

    /// A bit that indicates this block is passed directly to a function as a
    /// non-escaping parameter.
    uint64_t DoesNotEscape : 1;

    /// A bit that indicates whether it's possible to avoid coying this block to
    /// the heap when it initializes or is assigned to a local variable with
    /// automatic storage.
    uint64_t CanAvoidCopyToHeap : 1;
  };

  /// Number of non-inherited bits in BlockDeclBitfields.
  enum { NumBlockDeclBits = 5 };

  /// Pointer to the data structure used to lookup declarations
  /// within this context (or a DependentStoredDeclsMap if this is a
  /// dependent context). We maintain the invariant that, if the map
  /// contains an entry for a DeclarationName (and we haven't lazily
  /// omitted anything), then it contains all relevant entries for that
  /// name (modulo the hasExternalDecls() flag).
  mutable StoredDeclsMap *LookupPtr = nullptr;

protected:
  /// This anonymous union stores the bits belonging to DeclContext and classes
  /// deriving from it. The goal is to use otherwise wasted
  /// space in DeclContext to store data belonging to derived classes.
  /// The space saved is especially significient when pointers are aligned
  /// to 8 bytes. In this case due to alignment requirements we have a
  /// little less than 8 bytes free in DeclContext which we can use.
  /// We check that none of the classes in this union is larger than
  /// 8 bytes with static_asserts in the ctor of DeclContext.
  union {
    DeclContextBitfields DeclContextBits;
    TagDeclBitfields TagDeclBits;
    EnumDeclBitfields EnumDeclBits;
    RecordDeclBitfields RecordDeclBits;
    OMPDeclareReductionDeclBitfields OMPDeclareReductionDeclBits;
    FunctionDeclBitfields FunctionDeclBits;
    CXXConstructorDeclBitfields CXXConstructorDeclBits;
    ObjCMethodDeclBitfields ObjCMethodDeclBits;
    ObjCContainerDeclBitfields ObjCContainerDeclBits;
    LinkageSpecDeclBitfields LinkageSpecDeclBits;
    BlockDeclBitfields BlockDeclBits;

    static_assert(sizeof(DeclContextBitfields) <= 8,
                  "DeclContextBitfields is larger than 8 bytes!");
    static_assert(sizeof(TagDeclBitfields) <= 8,
                  "TagDeclBitfields is larger than 8 bytes!");
    static_assert(sizeof(EnumDeclBitfields) <= 8,
                  "EnumDeclBitfields is larger than 8 bytes!");
    static_assert(sizeof(RecordDeclBitfields) <= 8,
                  "RecordDeclBitfields is larger than 8 bytes!");
    static_assert(sizeof(OMPDeclareReductionDeclBitfields) <= 8,
                  "OMPDeclareReductionDeclBitfields is larger than 8 bytes!");
    static_assert(sizeof(FunctionDeclBitfields) <= 8,
                  "FunctionDeclBitfields is larger than 8 bytes!");
    static_assert(sizeof(CXXConstructorDeclBitfields) <= 8,
                  "CXXConstructorDeclBitfields is larger than 8 bytes!");
    static_assert(sizeof(ObjCMethodDeclBitfields) <= 8,
                  "ObjCMethodDeclBitfields is larger than 8 bytes!");
    static_assert(sizeof(ObjCContainerDeclBitfields) <= 8,
                  "ObjCContainerDeclBitfields is larger than 8 bytes!");
    static_assert(sizeof(LinkageSpecDeclBitfields) <= 8,
                  "LinkageSpecDeclBitfields is larger than 8 bytes!");
    static_assert(sizeof(BlockDeclBitfields) <= 8,
                  "BlockDeclBitfields is larger than 8 bytes!");
  };

  /// FirstDecl - The first declaration stored within this declaration
  /// context.
  mutable Decl *FirstDecl = nullptr;

  /// LastDecl - The last declaration stored within this declaration
  /// context. FIXME: We could probably cache this value somewhere
  /// outside of the DeclContext, to reduce the size of DeclContext by
  /// another pointer.
  mutable Decl *LastDecl = nullptr;

  /// Build up a chain of declarations.
  ///
  /// \returns the first/last pair of declarations.
  static std::pair<Decl *, Decl *>
  BuildDeclChain(ArrayRef<Decl*> Decls, bool FieldsAlreadyLoaded);

  DeclContext(Decl::Kind K);

public:
  ~DeclContext();

  Decl::Kind getDeclKind() const {
    return static_cast<Decl::Kind>(DeclContextBits.DeclKind);
  }

  const char *getDeclKindName() const;

  /// getParent - Returns the containing DeclContext.
  DeclContext *getParent() {
    return cast<Decl>(this)->getDeclContext();
  }
  const DeclContext *getParent() const {
    return const_cast<DeclContext*>(this)->getParent();
  }

  /// getLexicalParent - Returns the containing lexical DeclContext. May be
  /// different from getParent, e.g.:
  ///
  ///   namespace A {
  ///      struct S;
  ///   }
  ///   struct A::S {}; // getParent() == namespace 'A'
  ///                   // getLexicalParent() == translation unit
  ///
  DeclContext *getLexicalParent() {
    return cast<Decl>(this)->getLexicalDeclContext();
  }
  const DeclContext *getLexicalParent() const {
    return const_cast<DeclContext*>(this)->getLexicalParent();
  }

  DeclContext *getLookupParent();

  const DeclContext *getLookupParent() const {
    return const_cast<DeclContext*>(this)->getLookupParent();
  }

  ASTContext &getParentASTContext() const {
    return cast<Decl>(this)->getASTContext();
  }

  bool isClosure() const { return getDeclKind() == Decl::Block; }

  /// Return this DeclContext if it is a BlockDecl. Otherwise, return the
  /// innermost enclosing BlockDecl or null if there are no enclosing blocks.
  const BlockDecl *getInnermostBlockDecl() const;

  bool isObjCContainer() const {
    switch (getDeclKind()) {
    case Decl::ObjCCategory:
    case Decl::ObjCCategoryImpl:
    case Decl::ObjCImplementation:
    case Decl::ObjCInterface:
    case Decl::ObjCProtocol:
      return true;
    default:
      return false;
    }
  }

  bool isFunctionOrMethod() const {
    switch (getDeclKind()) {
    case Decl::Block:
    case Decl::Captured:
    case Decl::ObjCMethod:
      return true;
    default:
      return getDeclKind() >= Decl::firstFunction &&
             getDeclKind() <= Decl::lastFunction;
    }
  }

  /// Test whether the context supports looking up names.
  bool isLookupContext() const {
    return !isFunctionOrMethod() && getDeclKind() != Decl::LinkageSpec &&
           getDeclKind() != Decl::Export;
  }

  bool isFileContext() const {
    return getDeclKind() == Decl::TranslationUnit ||
           getDeclKind() == Decl::Namespace;
  }

  bool isTranslationUnit() const {
    return getDeclKind() == Decl::TranslationUnit;
  }

  bool isRecord() const {
    return getDeclKind() >= Decl::firstRecord &&
           getDeclKind() <= Decl::lastRecord;
  }

  bool isNamespace() const { return getDeclKind() == Decl::Namespace; }

  bool isStdNamespace() const;

  bool isInlineNamespace() const;

  /// Determines whether this context is dependent on a
  /// template parameter.
  bool isDependentContext() const;

  /// isTransparentContext - Determines whether this context is a
  /// "transparent" context, meaning that the members declared in this
  /// context are semantically declared in the nearest enclosing
  /// non-transparent (opaque) context but are lexically declared in
  /// this context. For example, consider the enumerators of an
  /// enumeration type:
  /// @code
  /// enum E {
  ///   Val1
  /// };
  /// @endcode
  /// Here, E is a transparent context, so its enumerator (Val1) will
  /// appear (semantically) that it is in the same context of E.
  /// Examples of transparent contexts include: enumerations (except for
  /// C++0x scoped enums), and C++ linkage specifications.
  bool isTransparentContext() const;

  /// Determines whether this context or some of its ancestors is a
  /// linkage specification context that specifies C linkage.
  bool isExternCContext() const;

  /// Retrieve the nearest enclosing C linkage specification context.
  const LinkageSpecDecl *getExternCContext() const;

  /// Determines whether this context or some of its ancestors is a
  /// linkage specification context that specifies C++ linkage.
  bool isExternCXXContext() const;

  /// Determine whether this declaration context is equivalent
  /// to the declaration context DC.
  bool Equals(const DeclContext *DC) const {
    return DC && this->getPrimaryContext() == DC->getPrimaryContext();
  }

  /// Determine whether this declaration context encloses the
  /// declaration context DC.
  bool Encloses(const DeclContext *DC) const;

  /// Find the nearest non-closure ancestor of this context,
  /// i.e. the innermost semantic parent of this context which is not
  /// a closure.  A context may be its own non-closure ancestor.
  Decl *getNonClosureAncestor();
  const Decl *getNonClosureAncestor() const {
    return const_cast<DeclContext*>(this)->getNonClosureAncestor();
  }

  /// getPrimaryContext - There may be many different
  /// declarations of the same entity (including forward declarations
  /// of classes, multiple definitions of namespaces, etc.), each with
  /// a different set of declarations. This routine returns the
  /// "primary" DeclContext structure, which will contain the
  /// information needed to perform name lookup into this context.
  DeclContext *getPrimaryContext();
  const DeclContext *getPrimaryContext() const {
    return const_cast<DeclContext*>(this)->getPrimaryContext();
  }

  /// getRedeclContext - Retrieve the context in which an entity conflicts with
  /// other entities of the same name, or where it is a redeclaration if the
  /// two entities are compatible. This skips through transparent contexts.
  DeclContext *getRedeclContext();
  const DeclContext *getRedeclContext() const {
    return const_cast<DeclContext *>(this)->getRedeclContext();
  }

  /// Retrieve the nearest enclosing namespace context.
  DeclContext *getEnclosingNamespaceContext();
  const DeclContext *getEnclosingNamespaceContext() const {
    return const_cast<DeclContext *>(this)->getEnclosingNamespaceContext();
  }

  /// Retrieve the outermost lexically enclosing record context.
  RecordDecl *getOuterLexicalRecordContext();
  const RecordDecl *getOuterLexicalRecordContext() const {
    return const_cast<DeclContext *>(this)->getOuterLexicalRecordContext();
  }

  /// Test if this context is part of the enclosing namespace set of
  /// the context NS, as defined in C++0x [namespace.def]p9. If either context
  /// isn't a namespace, this is equivalent to Equals().
  ///
  /// The enclosing namespace set of a namespace is the namespace and, if it is
  /// inline, its enclosing namespace, recursively.
  bool InEnclosingNamespaceSetOf(const DeclContext *NS) const;

  /// Collects all of the declaration contexts that are semantically
  /// connected to this declaration context.
  ///
  /// For declaration contexts that have multiple semantically connected but
  /// syntactically distinct contexts, such as C++ namespaces, this routine
  /// retrieves the complete set of such declaration contexts in source order.
  /// For example, given:
  ///
  /// \code
  /// namespace N {
  ///   int x;
  /// }
  /// namespace N {
  ///   int y;
  /// }
  /// \endcode
  ///
  /// The \c Contexts parameter will contain both definitions of N.
  ///
  /// \param Contexts Will be cleared and set to the set of declaration
  /// contexts that are semanticaly connected to this declaration context,
  /// in source order, including this context (which may be the only result,
  /// for non-namespace contexts).
  void collectAllContexts(SmallVectorImpl<DeclContext *> &Contexts);

  /// decl_iterator - Iterates through the declarations stored
  /// within this context.
  class decl_iterator {
    /// Current - The current declaration.
    Decl *Current = nullptr;

  public:
    using value_type = Decl *;
    using reference = const value_type &;
    using pointer = const value_type *;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;

    decl_iterator() = default;
    explicit decl_iterator(Decl *C) : Current(C) {}

    reference operator*() const { return Current; }

    // This doesn't meet the iterator requirements, but it's convenient
    value_type operator->() const { return Current; }

    decl_iterator& operator++() {
      Current = Current->getNextDeclInContext();
      return *this;
    }

    decl_iterator operator++(int) {
      decl_iterator tmp(*this);
      ++(*this);
      return tmp;
    }

    friend bool operator==(decl_iterator x, decl_iterator y) {
      return x.Current == y.Current;
    }

    friend bool operator!=(decl_iterator x, decl_iterator y) {
      return x.Current != y.Current;
    }
  };

  using decl_range = llvm::iterator_range<decl_iterator>;

  /// decls_begin/decls_end - Iterate over the declarations stored in
  /// this context.
  decl_range decls() const { return decl_range(decls_begin(), decls_end()); }
  decl_iterator decls_begin() const;
  decl_iterator decls_end() const { return decl_iterator(); }
  bool decls_empty() const;

  /// noload_decls_begin/end - Iterate over the declarations stored in this
  /// context that are currently loaded; don't attempt to retrieve anything
  /// from an external source.
  decl_range noload_decls() const {
    return decl_range(noload_decls_begin(), noload_decls_end());
  }
  decl_iterator noload_decls_begin() const { return decl_iterator(FirstDecl); }
  decl_iterator noload_decls_end() const { return decl_iterator(); }

  /// specific_decl_iterator - Iterates over a subrange of
  /// declarations stored in a DeclContext, providing only those that
  /// are of type SpecificDecl (or a class derived from it). This
  /// iterator is used, for example, to provide iteration over just
  /// the fields within a RecordDecl (with SpecificDecl = FieldDecl).
  template<typename SpecificDecl>
  class specific_decl_iterator {
    /// Current - The current, underlying declaration iterator, which
    /// will either be NULL or will point to a declaration of
    /// type SpecificDecl.
    DeclContext::decl_iterator Current;

    /// SkipToNextDecl - Advances the current position up to the next
    /// declaration of type SpecificDecl that also meets the criteria
    /// required by Acceptable.
    void SkipToNextDecl() {
      while (*Current && !isa<SpecificDecl>(*Current))
        ++Current;
    }

  public:
    using value_type = SpecificDecl *;
    // TODO: Add reference and pointer types (with some appropriate proxy type)
    // if we ever have a need for them.
    using reference = void;
    using pointer = void;
    using difference_type =
        std::iterator_traits<DeclContext::decl_iterator>::difference_type;
    using iterator_category = std::forward_iterator_tag;

    specific_decl_iterator() = default;

    /// specific_decl_iterator - Construct a new iterator over a
    /// subset of the declarations the range [C,
    /// end-of-declarations). If A is non-NULL, it is a pointer to a
    /// member function of SpecificDecl that should return true for
    /// all of the SpecificDecl instances that will be in the subset
    /// of iterators. For example, if you want Objective-C instance
    /// methods, SpecificDecl will be ObjCMethodDecl and A will be
    /// &ObjCMethodDecl::isInstanceMethod.
    explicit specific_decl_iterator(DeclContext::decl_iterator C) : Current(C) {
      SkipToNextDecl();
    }

    value_type operator*() const { return cast<SpecificDecl>(*Current); }

    // This doesn't meet the iterator requirements, but it's convenient
    value_type operator->() const { return **this; }

    specific_decl_iterator& operator++() {
      ++Current;
      SkipToNextDecl();
      return *this;
    }

    specific_decl_iterator operator++(int) {
      specific_decl_iterator tmp(*this);
      ++(*this);
      return tmp;
    }

    friend bool operator==(const specific_decl_iterator& x,
                           const specific_decl_iterator& y) {
      return x.Current == y.Current;
    }

    friend bool operator!=(const specific_decl_iterator& x,
                           const specific_decl_iterator& y) {
      return x.Current != y.Current;
    }
  };

  /// Iterates over a filtered subrange of declarations stored
  /// in a DeclContext.
  ///
  /// This iterator visits only those declarations that are of type
  /// SpecificDecl (or a class derived from it) and that meet some
  /// additional run-time criteria. This iterator is used, for
  /// example, to provide access to the instance methods within an
  /// Objective-C interface (with SpecificDecl = ObjCMethodDecl and
  /// Acceptable = ObjCMethodDecl::isInstanceMethod).
  template<typename SpecificDecl, bool (SpecificDecl::*Acceptable)() const>
  class filtered_decl_iterator {
    /// Current - The current, underlying declaration iterator, which
    /// will either be NULL or will point to a declaration of
    /// type SpecificDecl.
    DeclContext::decl_iterator Current;

    /// SkipToNextDecl - Advances the current position up to the next
    /// declaration of type SpecificDecl that also meets the criteria
    /// required by Acceptable.
    void SkipToNextDecl() {
      while (*Current &&
             (!isa<SpecificDecl>(*Current) ||
              (Acceptable && !(cast<SpecificDecl>(*Current)->*Acceptable)())))
        ++Current;
    }

  public:
    using value_type = SpecificDecl *;
    // TODO: Add reference and pointer types (with some appropriate proxy type)
    // if we ever have a need for them.
    using reference = void;
    using pointer = void;
    using difference_type =
        std::iterator_traits<DeclContext::decl_iterator>::difference_type;
    using iterator_category = std::forward_iterator_tag;

    filtered_decl_iterator() = default;

    /// filtered_decl_iterator - Construct a new iterator over a
    /// subset of the declarations the range [C,
    /// end-of-declarations). If A is non-NULL, it is a pointer to a
    /// member function of SpecificDecl that should return true for
    /// all of the SpecificDecl instances that will be in the subset
    /// of iterators. For example, if you want Objective-C instance
    /// methods, SpecificDecl will be ObjCMethodDecl and A will be
    /// &ObjCMethodDecl::isInstanceMethod.
    explicit filtered_decl_iterator(DeclContext::decl_iterator C) : Current(C) {
      SkipToNextDecl();
    }

    value_type operator*() const { return cast<SpecificDecl>(*Current); }
    value_type operator->() const { return cast<SpecificDecl>(*Current); }

    filtered_decl_iterator& operator++() {
      ++Current;
      SkipToNextDecl();
      return *this;
    }

    filtered_decl_iterator operator++(int) {
      filtered_decl_iterator tmp(*this);
      ++(*this);
      return tmp;
    }

    friend bool operator==(const filtered_decl_iterator& x,
                           const filtered_decl_iterator& y) {
      return x.Current == y.Current;
    }

    friend bool operator!=(const filtered_decl_iterator& x,
                           const filtered_decl_iterator& y) {
      return x.Current != y.Current;
    }
  };

  /// Add the declaration D into this context.
  ///
  /// This routine should be invoked when the declaration D has first
  /// been declared, to place D into the context where it was
  /// (lexically) defined. Every declaration must be added to one
  /// (and only one!) context, where it can be visited via
  /// [decls_begin(), decls_end()). Once a declaration has been added
  /// to its lexical context, the corresponding DeclContext owns the
  /// declaration.
  ///
  /// If D is also a NamedDecl, it will be made visible within its
  /// semantic context via makeDeclVisibleInContext.
  void addDecl(Decl *D);

  /// Add the declaration D into this context, but suppress
  /// searches for external declarations with the same name.
  ///
  /// Although analogous in function to addDecl, this removes an
  /// important check.  This is only useful if the Decl is being
  /// added in response to an external search; in all other cases,
  /// addDecl() is the right function to use.
  /// See the ASTImporter for use cases.
  void addDeclInternal(Decl *D);

  /// Add the declaration D to this context without modifying
  /// any lookup tables.
  ///
  /// This is useful for some operations in dependent contexts where
  /// the semantic context might not be dependent;  this basically
  /// only happens with friends.
  void addHiddenDecl(Decl *D);

  /// Removes a declaration from this context.
  void removeDecl(Decl *D);

  /// Checks whether a declaration is in this context.
  bool containsDecl(Decl *D) const;

  /// Checks whether a declaration is in this context.
  /// This also loads the Decls from the external source before the check.
  bool containsDeclAndLoad(Decl *D) const;

  using lookup_result = DeclContextLookupResult;
  using lookup_iterator = lookup_result::iterator;

  /// lookup - Find the declarations (if any) with the given Name in
  /// this context. Returns a range of iterators that contains all of
  /// the declarations with this name, with object, function, member,
  /// and enumerator names preceding any tag name. Note that this
  /// routine will not look into parent contexts.
  lookup_result lookup(DeclarationName Name) const;

  /// Find the declarations with the given name that are visible
  /// within this context; don't attempt to retrieve anything from an
  /// external source.
  lookup_result noload_lookup(DeclarationName Name);

  /// A simplistic name lookup mechanism that performs name lookup
  /// into this declaration context without consulting the external source.
  ///
  /// This function should almost never be used, because it subverts the
  /// usual relationship between a DeclContext and the external source.
  /// See the ASTImporter for the (few, but important) use cases.
  ///
  /// FIXME: This is very inefficient; replace uses of it with uses of
  /// noload_lookup.
  void localUncachedLookup(DeclarationName Name,
                           SmallVectorImpl<NamedDecl *> &Results);

  /// Makes a declaration visible within this context.
  ///
  /// This routine makes the declaration D visible to name lookup
  /// within this context and, if this is a transparent context,
  /// within its parent contexts up to the first enclosing
  /// non-transparent context. Making a declaration visible within a
  /// context does not transfer ownership of a declaration, and a
  /// declaration can be visible in many contexts that aren't its
  /// lexical context.
  ///
  /// If D is a redeclaration of an existing declaration that is
  /// visible from this context, as determined by
  /// NamedDecl::declarationReplaces, the previous declaration will be
  /// replaced with D.
  void makeDeclVisibleInContext(NamedDecl *D);

  /// all_lookups_iterator - An iterator that provides a view over the results
  /// of looking up every possible name.
  class all_lookups_iterator;

  using lookups_range = llvm::iterator_range<all_lookups_iterator>;

  lookups_range lookups() const;
  // Like lookups(), but avoids loading external declarations.
  // If PreserveInternalState, avoids building lookup data structures too.
  lookups_range noload_lookups(bool PreserveInternalState) const;

  /// Iterators over all possible lookups within this context.
  all_lookups_iterator lookups_begin() const;
  all_lookups_iterator lookups_end() const;

  /// Iterators over all possible lookups within this context that are
  /// currently loaded; don't attempt to retrieve anything from an external
  /// source.
  all_lookups_iterator noload_lookups_begin() const;
  all_lookups_iterator noload_lookups_end() const;

  struct udir_iterator;

  using udir_iterator_base =
      llvm::iterator_adaptor_base<udir_iterator, lookup_iterator,
                                  std::random_access_iterator_tag,
                                  UsingDirectiveDecl *>;

  struct udir_iterator : udir_iterator_base {
    udir_iterator(lookup_iterator I) : udir_iterator_base(I) {}

    UsingDirectiveDecl *operator*() const;
  };

  using udir_range = llvm::iterator_range<udir_iterator>;

  udir_range using_directives() const;

  // These are all defined in DependentDiagnostic.h.
  class ddiag_iterator;

  using ddiag_range = llvm::iterator_range<DeclContext::ddiag_iterator>;

  inline ddiag_range ddiags() const;

  // Low-level accessors

  /// Mark that there are external lexical declarations that we need
  /// to include in our lookup table (and that are not available as external
  /// visible lookups). These extra lookup results will be found by walking
  /// the lexical declarations of this context. This should be used only if
  /// setHasExternalLexicalStorage() has been called on any decl context for
  /// which this is the primary context.
  void setMustBuildLookupTable() {
    assert(this == getPrimaryContext() &&
           "should only be called on primary context");
    DeclContextBits.HasLazyExternalLexicalLookups = true;
  }

  /// Retrieve the internal representation of the lookup structure.
  /// This may omit some names if we are lazily building the structure.
  StoredDeclsMap *getLookupPtr() const { return LookupPtr; }

  /// Ensure the lookup structure is fully-built and return it.
  StoredDeclsMap *buildLookup();

  /// Whether this DeclContext has external storage containing
  /// additional declarations that are lexically in this context.
  bool hasExternalLexicalStorage() const {
    return DeclContextBits.ExternalLexicalStorage;
  }

  /// State whether this DeclContext has external storage for
  /// declarations lexically in this context.
  void setHasExternalLexicalStorage(bool ES = true) const {
    DeclContextBits.ExternalLexicalStorage = ES;
  }

  /// Whether this DeclContext has external storage containing
  /// additional declarations that are visible in this context.
  bool hasExternalVisibleStorage() const {
    return DeclContextBits.ExternalVisibleStorage;
  }

  /// State whether this DeclContext has external storage for
  /// declarations visible in this context.
  void setHasExternalVisibleStorage(bool ES = true) const {
    DeclContextBits.ExternalVisibleStorage = ES;
    if (ES && LookupPtr)
      DeclContextBits.NeedToReconcileExternalVisibleStorage = true;
  }

  /// Determine whether the given declaration is stored in the list of
  /// declarations lexically within this context.
  bool isDeclInLexicalTraversal(const Decl *D) const {
    return D && (D->NextInContextAndBits.getPointer() || D == FirstDecl ||
                 D == LastDecl);
  }

  bool setUseQualifiedLookup(bool use = true) const {
    bool old_value = DeclContextBits.UseQualifiedLookup;
    DeclContextBits.UseQualifiedLookup = use;
    return old_value;
  }

  bool shouldUseQualifiedLookup() const {
    return DeclContextBits.UseQualifiedLookup;
  }

  static bool classof(const Decl *D);
  static bool classof(const DeclContext *D) { return true; }

  void dumpDeclContext() const;
  void dumpLookups() const;
  void dumpLookups(llvm::raw_ostream &OS, bool DumpDecls = false,
                   bool Deserialize = false) const;

private:
  /// Whether this declaration context has had externally visible
  /// storage added since the last lookup. In this case, \c LookupPtr's
  /// invariant may not hold and needs to be fixed before we perform
  /// another lookup.
  bool hasNeedToReconcileExternalVisibleStorage() const {
    return DeclContextBits.NeedToReconcileExternalVisibleStorage;
  }

  /// State that this declaration context has had externally visible
  /// storage added since the last lookup. In this case, \c LookupPtr's
  /// invariant may not hold and needs to be fixed before we perform
  /// another lookup.
  void setNeedToReconcileExternalVisibleStorage(bool Need = true) const {
    DeclContextBits.NeedToReconcileExternalVisibleStorage = Need;
  }

  /// If \c true, this context may have local lexical declarations
  /// that are missing from the lookup table.
  bool hasLazyLocalLexicalLookups() const {
    return DeclContextBits.HasLazyLocalLexicalLookups;
  }

  /// If \c true, this context may have local lexical declarations
  /// that are missing from the lookup table.
  void setHasLazyLocalLexicalLookups(bool HasLLLL = true) const {
    DeclContextBits.HasLazyLocalLexicalLookups = HasLLLL;
  }

  /// If \c true, the external source may have lexical declarations
  /// that are missing from the lookup table.
  bool hasLazyExternalLexicalLookups() const {
    return DeclContextBits.HasLazyExternalLexicalLookups;
  }

  /// If \c true, the external source may have lexical declarations
  /// that are missing from the lookup table.
  void setHasLazyExternalLexicalLookups(bool HasLELL = true) const {
    DeclContextBits.HasLazyExternalLexicalLookups = HasLELL;
  }

  void reconcileExternalVisibleStorage() const;
  bool LoadLexicalDeclsFromExternalStorage() const;

  /// Makes a declaration visible within this context, but
  /// suppresses searches for external declarations with the same
  /// name.
  ///
  /// Analogous to makeDeclVisibleInContext, but for the exclusive
  /// use of addDeclInternal().
  void makeDeclVisibleInContextInternal(NamedDecl *D);

  StoredDeclsMap *CreateStoredDeclsMap(ASTContext &C) const;

  void loadLazyLocalLexicalLookups();
  void buildLookupImpl(DeclContext *DCtx, bool Internal);
  void makeDeclVisibleInContextWithFlags(NamedDecl *D, bool Internal,
                                         bool Rediscoverable);
  void makeDeclVisibleInContextImpl(NamedDecl *D, bool Internal);
};

inline bool Decl::isTemplateParameter() const {
  return getKind() == TemplateTypeParm || getKind() == NonTypeTemplateParm ||
         getKind() == TemplateTemplateParm;
}

// Specialization selected when ToTy is not a known subclass of DeclContext.
template <class ToTy,
          bool IsKnownSubtype = ::std::is_base_of<DeclContext, ToTy>::value>
struct cast_convert_decl_context {
  static const ToTy *doit(const DeclContext *Val) {
    return static_cast<const ToTy*>(Decl::castFromDeclContext(Val));
  }

  static ToTy *doit(DeclContext *Val) {
    return static_cast<ToTy*>(Decl::castFromDeclContext(Val));
  }
};

// Specialization selected when ToTy is a known subclass of DeclContext.
template <class ToTy>
struct cast_convert_decl_context<ToTy, true> {
  static const ToTy *doit(const DeclContext *Val) {
    return static_cast<const ToTy*>(Val);
  }

  static ToTy *doit(DeclContext *Val) {
    return static_cast<ToTy*>(Val);
  }
};

} // namespace clang

namespace llvm {

/// isa<T>(DeclContext*)
template <typename To>
struct isa_impl<To, ::clang::DeclContext> {
  static bool doit(const ::clang::DeclContext &Val) {
    return To::classofKind(Val.getDeclKind());
  }
};

/// cast<T>(DeclContext*)
template<class ToTy>
struct cast_convert_val<ToTy,
                        const ::clang::DeclContext,const ::clang::DeclContext> {
  static const ToTy &doit(const ::clang::DeclContext &Val) {
    return *::clang::cast_convert_decl_context<ToTy>::doit(&Val);
  }
};

template<class ToTy>
struct cast_convert_val<ToTy, ::clang::DeclContext, ::clang::DeclContext> {
  static ToTy &doit(::clang::DeclContext &Val) {
    return *::clang::cast_convert_decl_context<ToTy>::doit(&Val);
  }
};

template<class ToTy>
struct cast_convert_val<ToTy,
                     const ::clang::DeclContext*, const ::clang::DeclContext*> {
  static const ToTy *doit(const ::clang::DeclContext *Val) {
    return ::clang::cast_convert_decl_context<ToTy>::doit(Val);
  }
};

template<class ToTy>
struct cast_convert_val<ToTy, ::clang::DeclContext*, ::clang::DeclContext*> {
  static ToTy *doit(::clang::DeclContext *Val) {
    return ::clang::cast_convert_decl_context<ToTy>::doit(Val);
  }
};

/// Implement cast_convert_val for Decl -> DeclContext conversions.
template<class FromTy>
struct cast_convert_val< ::clang::DeclContext, FromTy, FromTy> {
  static ::clang::DeclContext &doit(const FromTy &Val) {
    return *FromTy::castToDeclContext(&Val);
  }
};

template<class FromTy>
struct cast_convert_val< ::clang::DeclContext, FromTy*, FromTy*> {
  static ::clang::DeclContext *doit(const FromTy *Val) {
    return FromTy::castToDeclContext(Val);
  }
};

template<class FromTy>
struct cast_convert_val< const ::clang::DeclContext, FromTy, FromTy> {
  static const ::clang::DeclContext &doit(const FromTy &Val) {
    return *FromTy::castToDeclContext(&Val);
  }
};

template<class FromTy>
struct cast_convert_val< const ::clang::DeclContext, FromTy*, FromTy*> {
  static const ::clang::DeclContext *doit(const FromTy *Val) {
    return FromTy::castToDeclContext(Val);
  }
};

} // namespace llvm

#endif // LLVM_CLANG_AST_DECLBASE_H
