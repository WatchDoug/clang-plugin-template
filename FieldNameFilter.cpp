#include "clang/Driver/Options.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/FileUtilities.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"

using namespace clang;

std::vector<std::string> Filter = {"min", "max", "nr", "ratio"};
namespace{
class FieldNameFilterVisitor : public RecursiveASTVisitor<FieldNameFilterVisitor> {
	public:
		explicit FieldNameFilterVisitor(ASTContext *Context) : _astContext_(Context) {}
	 	bool VisitFieldDecl(FieldDecl *D) {
			std::error_code ec;
			llvm::raw_fd_ostream fstream("./field-filter.txt", ec, llvm::sys::fs::OF_Text | llvm::sys::fs::OF_Append);
			auto name = D->getNameAsString();
			bool istarget = false;
			for (int i = 0; i < Filter.size(); ++i) {
				if (name.find(Filter[i]) != std::string::npos)
					istarget = true;
			}
			if (istarget){
				D->printQualifiedName(fstream);
				fstream << '\n';
			}
			return true;
		}	
	private:
		ASTContext *_astContext_;
};


class FieldNameFilterASTConsumer : public clang::ASTConsumer {
	public:
		explicit FieldNameFilterASTConsumer(ASTContext *Context) : _visitor_(Context) {}
		virtual void HandleTranslationUnit(ASTContext &Context) {
			_visitor_.TraverseDecl(Context.getTranslationUnitDecl());
		}
	private:
		FieldNameFilterVisitor _visitor_;
};


class FieldNameFilterAction : public clang::PluginASTAction {
	protected:
		std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) {
			return std::make_unique<FieldNameFilterASTConsumer>(&CI.getASTContext());
		}
		     
		bool ParseArgs(const CompilerInstance &CI, const std::vector<std::string> &args) override {
			return true;
		}

		PluginASTAction::ActionType getActionType() override {
			return AddAfterMainAction;
		}
};

}

static FrontendPluginRegistry::Add<FieldNameFilterAction>
X("field-name-filter", "filter struct fields by specific names");
