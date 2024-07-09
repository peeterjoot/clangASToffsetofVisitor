#include <iostream>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/AST/RecordLayout.h>
#include <clang/Rewrite/Frontend/FrontendActions.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/Tooling.h>

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

class OffsetOfVisitor : public RecursiveASTVisitor<OffsetOfVisitor> {
public:
    explicit OffsetOfVisitor(ASTContext *Context)
        : Context(Context) {}

    bool VisitFieldDecl(FieldDecl *FD) {
        const RecordDecl *Parent = FD->getParent();
        if (!Parent) {
            llvm::errs() << "Parent record is null for field: " << FD->getNameAsString() << "\n";
            return true;
        }

        if (Parent->isDependentType() || Parent->isInvalidDecl()) {
            llvm::errs() << "Parent record is a dependent or invalid type for field: " << FD->getNameAsString() << "\n";
            return true;
        }

        const ASTRecordLayout &Layout = Context->getASTRecordLayout(Parent);

        std::string FieldName = FD->getNameAsString();
        if (FieldName.empty()) {
            FieldName = "(anonymous)";
        }

        if (const clang::CXXRecordDecl *CXXRD = llvm::dyn_cast<clang::CXXRecordDecl>(Parent)) {
            for (const clang::CXXBaseSpecifier &Base : CXXRD->bases()) {
                const CXXRecordDecl *BaseDecl = Base.getType()->getAsCXXRecordDecl();
                if (!BaseDecl) {
                    llvm::errs() << "BaseDecl is null for field: " << FieldName << "\n";
                    continue;
                }
                if (BaseDecl->isDependentType() || BaseDecl->isInvalidDecl()) {
                    llvm::errs() << "BaseDecl is a dependent or invalid type for field: " << FieldName << "\n";
                    continue;
                }
                //const ASTRecordLayout &BaseLayout = Context->getASTRecordLayout(BaseDecl);
            }
        }

        std::string ParentName;
        auto off = Layout.getFieldOffset(FD->getFieldIndex()) / 8;

        // Check for a typedef name first
        if (const TypedefNameDecl *TND = Parent->getTypedefNameForAnonDecl()) {
            ParentName = TND->getNameAsString();
        }

        // If the name is still empty, check the tag declaration
        if (ParentName.empty()) {
            if (const TagDecl *TD = dyn_cast<TagDecl>(Parent)) {
                ParentName = TD->getNameAsString();
            }
        }

        // If the name is still empty, use "(anonymous)" as a placeholder, unless the field also has offset 0 (probably entirely an anonymous union.)
        if (ParentName.empty()) {
            ParentName = "(anonymous)";
            if ( off == 0 ) {
                return true;
            }
        }

        llvm::outs() << "offsetof(" << ParentName << ", " << FieldName << ") = " << off << "\n";

        return true;
    }

private:
    ASTContext *Context;
};

class OffsetOfConsumer : public ASTConsumer {
public:
    explicit OffsetOfConsumer(ASTContext *Context)
        : Visitor(Context) {}

    void HandleTranslationUnit(ASTContext &Context) override {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    OffsetOfVisitor Visitor;
};

class OffsetOfAction : public ASTFrontendAction {
public:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef File) override {
        return std::make_unique<OffsetOfConsumer>(&CI.getASTContext());
    }
};

// Set up the command-line options
static llvm::cl::OptionCategory MyToolCategory("my-tool options");

int main(int argc, const char **argv) {
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }
    CommonOptionsParser &OptionsParser = ExpectedParser.get();
    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

    // Add a debug statement to check source paths
    for (const auto &Path : OptionsParser.getSourcePathList()) {
        std::cout << "Processing file: " << Path << std::endl;
    }

    // Debugging the ClangTool execution
    int result = Tool.run(newFrontendActionFactory<OffsetOfAction>().get());

    std::cout << "ClangTool execution completed with result: " << result << std::endl;
    return result;
}

// vim: et ts=4 sw=4
