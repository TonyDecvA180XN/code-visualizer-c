#pragma once

#include "Core.h"

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Rewrite/Core/Rewriter.h"

class VisualizerVisitor : public clang::RecursiveASTVisitor<VisualizerVisitor>
{
public:
	VisualizerVisitor(clang::ASTContext& context, clang::Rewriter& rewriter, SymbolTable& symbolTable);

	bool VisitVarDecl(clang::VarDecl* declaration);
	bool VisitRecordDecl(clang::RecordDecl* declaration);
	bool VisitFieldDecl(clang::FieldDecl* declaration);
	// TODO: get include location
	// fullLocation.getIncludeLoc()
	bool VisitFunctionDecl(clang::FunctionDecl* declaration);
	bool VisitDeclRefExpr(clang::DeclRefExpr* expression);
	bool VisitCStyleCastExpr(clang::CStyleCastExpr* expression);
	bool VisitCallExpr(clang::CallExpr* expression);
	bool VisitUnaryOperator(clang::UnaryOperator* oper);
	bool VisitBinaryOperator(clang::BinaryOperator* oper);
	bool VisitIfStmt(clang::IfStmt* statement);
	bool VisitSwitchStmt(clang::SwitchStmt* statement);
	bool VisitCaseStmt(clang::CaseStmt* statement);
	bool VisitDefaultStmt(clang::DefaultStmt* statement);
	bool VisitForStmt(clang::ForStmt* statement);
	bool VisitWhileStmt(clang::WhileStmt* statement);
	bool VisitDoStmt(clang::DoStmt* statement);
	bool VisitBreakStmt(clang::BreakStmt* statement);
	bool VisitContinueStmt(clang::ContinueStmt* statement);
	bool VisitReturnStmt(clang::ReturnStmt* statement);
	bool VisitIntegerLiteral(clang::IntegerLiteral* literal);
	bool VisitFloatingLiteral(clang::FloatingLiteral* literal);
	bool VisitStringLiteral(clang::StringLiteral* literal);

private:
	clang::ASTContext& mContext;
	clang::Rewriter& mRewriter;
	SymbolTable& mSymbolTable;
};
