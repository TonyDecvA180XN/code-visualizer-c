#include "VisualizerVisitor.h"

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"

#include <Core.h>
#include <MarkupTree.h>
#include <filesystem>

using namespace clang;
using namespace Quote;

VisualizerVisitor::VisualizerVisitor(ASTContext& context, Rewriter& rewriter, SymbolTable& symbolTable) :
	mContext(context),
	mRewriter(rewriter),
	mSymbolTable(symbolTable)
{
}

bool VisualizerVisitor::VisitVarDecl(VarDecl* declaration)
{
	if (mContext.getSourceManager().isInSystemHeader(declaration->getLocation()))
		return true;

	mRewriter.InsertText(declaration->getBeginLoc(), MakeOpenSpan("code-keyword-declaration"));
	mRewriter.InsertTextAfterToken(declaration->getLocation(), std::format("</span>"));

	mRewriter.InsertText(declaration->getTypeSpecStartLoc(), MakeOpenSpan("code-type-user"));
	mRewriter.InsertTextAfterToken(declaration->getTypeSpecEndLoc(), std::format("</span>"));

	std::string name = declaration->getNameAsString();

	mRewriter.InsertText(declaration->getLocation(), MakeOpenSpan(std::format("code-id-var symbol-{}", name)));
	mRewriter.InsertTextAfterToken(declaration->getLocation(), std::format("</span>"));

	SourceLocation from = declaration->getBeginLoc(), to = declaration->getEndLoc();
	PresumedLoc fromDesc = mContext.getFullLoc(from).getPresumedLoc();
	PresumedLoc toDesc = mContext.getFullLoc(to).getPresumedLoc();
	std::string filename = UniversalLocalPath(fromDesc.getFilename()).string();

	mSymbolTable[name] = DefinitionLocation{filename, "variable", fromDesc.getLine(), toDesc.getLine()};

	return true;
}

bool VisualizerVisitor::VisitRecordDecl(RecordDecl* declaration)
{
	if (mContext.getSourceManager().isInSystemHeader(declaration->getLocation()))
		return true;

	SourceLocation nameLocation = declaration->getLocation();
	SourceLocation keywordLocation = declaration->getBeginLoc();

	std::string name = declaration->getNameAsString();
	std::string keyword = "struct";

	mRewriter.ReplaceText(keywordLocation, keyword.size(), MakeSpan("code-keyword-decl", keyword));
	mRewriter.ReplaceText(nameLocation, name.size(), MakeSpan("code-type-user", name));

	SourceLocation from = declaration->getBeginLoc(), to = declaration->getEndLoc();
	PresumedLoc fromDesc = mContext.getFullLoc(from).getPresumedLoc();
	PresumedLoc toDesc = mContext.getFullLoc(to).getPresumedLoc();
	std::string filename = UniversalLocalPath(fromDesc.getFilename()).string();

	mSymbolTable[name] = DefinitionLocation{filename, "struct", fromDesc.getLine(), toDesc.getLine()};

	return true;
}

bool VisualizerVisitor::VisitFieldDecl(FieldDecl* declaration)
{
	if (mContext.getSourceManager().isInSystemHeader(declaration->getLocation()))
		return true;

	mRewriter.InsertText(declaration->getTypeSpecStartLoc(), MakeOpenSpan("code-type-user"));
	mRewriter.InsertTextAfterToken(declaration->getTypeSpecEndLoc(), std::format("</span>"));

	mRewriter.InsertText(declaration->getLocation(), MakeOpenSpan("code-id-var"));
	mRewriter.InsertTextAfterToken(declaration->getLocation(), std::format("</span>"));

	std::string name = declaration->getNameAsString();
	SourceLocation from = declaration->getBeginLoc(), to = declaration->getEndLoc();
	PresumedLoc fromDesc = mContext.getFullLoc(from).getPresumedLoc();
	PresumedLoc toDesc = mContext.getFullLoc(to).getPresumedLoc();
	std::string filename = UniversalLocalPath(fromDesc.getFilename()).string();

	mSymbolTable[name] = DefinitionLocation{filename, "member", fromDesc.getLine(), toDesc.getLine()};

	return true;
}

bool VisualizerVisitor::VisitFunctionDecl(FunctionDecl* declaration)
{
	SourceLocation location = declaration->getLocation();

	if (mContext.getSourceManager().isInSystemHeader(location))
		return true;

	std::string name = declaration->getNameAsString();
	std::string returnType = declaration->getReturnType().getAsString();
	mRewriter.ReplaceText(declaration->getBeginLoc(), returnType.size(), MakeSpan("code-type-user", returnType));

	if (declaration->isThisDeclarationADefinition())
	{
		SourceLocation from = declaration->getBeginLoc(), to = declaration->getEndLoc();
		PresumedLoc fromDesc = mContext.getFullLoc(from).getPresumedLoc();
		PresumedLoc toDesc = mContext.getFullLoc(to).getPresumedLoc();
		std::string filename = UniversalLocalPath(fromDesc.getFilename()).string();

		mSymbolTable[name] = DefinitionLocation{filename, "function", fromDesc.getLine(), toDesc.getLine()};

		mRewriter.ReplaceText(location, name.size(), MakeSpan(std::format("code-id-fun symbol-{}", name), name));
	}
	else
	{
		MarkupTree invokeMenu("span");
		MarkupTreeNode& invokeGroup = invokeMenu.GetRoot();
		invokeGroup.AddAttribute("class", "dropdown");

		MarkupTreeNode& symbol = invokeGroup.AppendChild("span");
		symbol.AddAttribute("class", std::format("code-id-fun symbol-{}", name));
		symbol.AddAttribute("onclick", std::format("showDropdown(\'{}\')", std::format("dropdown-{}", name)));
		symbol.SetText(name);

		MarkupTreeNode& dropdownContent = invokeGroup.AppendChild("span");
		dropdownContent.AddAttribute("class", "dropdown-content hidden");
		dropdownContent.AddAttribute("id", std::format("dropdown-{}", name));

		MarkupTreeNode& gotToDefinition = dropdownContent.AppendChild("span");
		gotToDefinition.AddAttribute("onclick", std::format("goToDefinition(\'{}\')", name));
		gotToDefinition.SetText("Go to definition");

		std::string tag = invokeMenu.ToString(false);
		mRewriter.ReplaceText(location, name.size(), ReplaceAll(tag, "\"", SPECIAL_STRING));
	}
	return true;
}

bool VisualizerVisitor::VisitDeclRefExpr(DeclRefExpr* expression)
{
	SourceLocation location = expression->getLocation();

	if (VarDecl* varDecl = dyn_cast<VarDecl>(expression->getDecl()))
	{
		std::string name = varDecl->getNameAsString();
		PresumedLoc varDeclLocation = mContext.getFullLoc(varDecl->getLocation()).getPresumedLoc();
		PresumedLoc varRefLocation = mContext.getFullLoc(location).getPresumedLoc();

		std::string filename = UniversalLocalPath(varDeclLocation.getFilename()).string();

		MarkupTree invokeMenu("span");
		MarkupTreeNode& invokeGroup = invokeMenu.GetRoot();
		invokeGroup.AddAttribute("class", "dropdown");

		MarkupTreeNode& symbol = invokeGroup.AppendChild("span");
		symbol.AddAttribute("class", std::format("code-id-var symbol-{}", name));
		symbol.AddAttribute(
			"onclick",
			std::format("showDropdown(\'{}\')", std::format("dropdown-{}-{}-{}", name, varRefLocation.getLine(), varRefLocation.getColumn())));
		symbol.SetText(name);

		MarkupTreeNode& dropdownContent = invokeGroup.AppendChild("span");
		dropdownContent.AddAttribute("class", "dropdown-content hidden");
		dropdownContent.AddAttribute("id", std::format("dropdown-{}-{}-{}", name, varRefLocation.getLine(), varRefLocation.getColumn()));

		MarkupTreeNode& goToDec = dropdownContent.AppendChild("span");
		goToDec.AddAttribute("onclick", std::format("goToDeclaration(\'{}\', \'{}\')", filename, varDeclLocation.getLine()));
		goToDec.SetText("Go to declaration");

		std::string tag = invokeMenu.ToString(false);
		mRewriter.ReplaceText(location, name.size(), ReplaceAll(tag, "\"", SPECIAL_STRING));
	}

	return true;
}

bool VisualizerVisitor::VisitCStyleCastExpr(CStyleCastExpr* expression)
{
	mRewriter.InsertTextAfterToken(expression->getLParenLoc(), MakeOpenSpan("code-type-user"));
	mRewriter.InsertText(expression->getRParenLoc(), std::format("</span>"));
	return true;
}

bool VisualizerVisitor::VisitCallExpr(CallExpr* expression)
{
	Decl* decl = expression->getCalleeDecl();
	FunctionDecl* funDecl = decl->getAsFunction();
	if (funDecl)
	{
		std::string name = funDecl->getNameAsString();
		SourceLocation callLocation = expression->getBeginLoc();
		SourceLocation functionLocation = funDecl->getLocation();
		PresumedLoc funDeclLocation = mContext.getFullLoc(functionLocation).getPresumedLoc();
		PresumedLoc funRefLocation = mContext.getFullLoc(callLocation).getPresumedLoc();
		assert(funDeclLocation.isValid());

		if (mContext.getSourceManager().isInSystemHeader(functionLocation))
		{
			mRewriter.ReplaceText(callLocation, name.size(), MakeSpan("code-id-fun", name));
			return true;
		}

		std::string filename = UniversalLocalPath(funDeclLocation.getFilename()).string();

		MarkupTree invokeMenu("span");
		MarkupTreeNode& invokeGroup = invokeMenu.GetRoot();
		invokeGroup.AddAttribute("class", "dropdown");

		MarkupTreeNode& symbol = invokeGroup.AppendChild("span");
		symbol.AddAttribute("class", std::format("code-id-fun symbol-{}", name));
		symbol.AddAttribute(
			"onclick",
			std::format("showDropdown(\'{}\')", std::format("dropdown-{}-{}-{}", name, funRefLocation.getLine(), funRefLocation.getColumn())));
		symbol.SetText(name);

		MarkupTreeNode& dropdownContent = invokeGroup.AppendChild("span");
		dropdownContent.AddAttribute("class", "dropdown-content hidden");
		dropdownContent.AddAttribute("id", std::format("dropdown-{}-{}-{}", name, funRefLocation.getLine(), funRefLocation.getColumn()));

		MarkupTreeNode& goToDec = dropdownContent.AppendChild("span");
		goToDec.AddAttribute("onclick", std::format("goToDeclaration(\'{}\', \'{}\')", filename, funDeclLocation.getLine()));
		goToDec.SetText("Go to declaration");
		MarkupTreeNode& goToDef = dropdownContent.AppendChild("span");
		goToDef.AddAttribute("onclick", std::format("goToDefinition(\'{}\')", name));
		goToDef.SetText("Go to definition");

		std::string tag = invokeMenu.ToString(false);
		mRewriter.ReplaceText(callLocation, name.size(), ReplaceAll(tag, "\"", SPECIAL_STRING));

		//mRewriter.ReplaceText(callLocation, name.size(),
		//std::format("<a class={}code-id-fun{} onclick={}goToDeclaration(\'{}\'){}>{}</a>", q, q, q, path.string(), q, name));
	}
	return true;
}

bool VisualizerVisitor::VisitUnaryOperator(UnaryOperator* oper)
{
	if (oper->getOpcode() == UnaryOperatorKind::UO_AddrOf)
	{
		mRewriter.ReplaceText(oper->getOperatorLoc(), 1, "&amp;");
	}
	return true;
}

bool VisualizerVisitor::VisitBinaryOperator(BinaryOperator* oper)
{
	if (oper->getOpcode() == BinaryOperatorKind::BO_LT || oper->getOpcode() == BinaryOperatorKind::BO_LE)
	{
		mRewriter.ReplaceText(oper->getOperatorLoc(), 1, "&lt;");
	}
	if (oper->getOpcode() == BinaryOperatorKind::BO_GT || oper->getOpcode() == BinaryOperatorKind::BO_GE)
	{
		mRewriter.ReplaceText(oper->getOperatorLoc(), 1, "&gt;");
	}
	return true;
}

bool VisualizerVisitor::VisitIfStmt(IfStmt* statement)
{
	std::string name = "if";
	mRewriter.ReplaceText(statement->getBeginLoc(), name.size(), MakeSpan("code-keyword-oper", name));
	if (statement->getElse())
	{
		name = "else";
		mRewriter.ReplaceText(statement->getElseLoc(), name.size(), MakeSpan("code-keyword-oper", name));
	}
	return true;
}

bool VisualizerVisitor::VisitSwitchStmt(SwitchStmt* statement)
{
	std::string name = "switch";
	mRewriter.ReplaceText(statement->getBeginLoc(), name.size(), MakeSpan("code-keyword-oper", name));
	return true;
}

bool VisualizerVisitor::VisitCaseStmt(CaseStmt* statement)
{
	std::string name = "case";
	mRewriter.ReplaceText(statement->getBeginLoc(), name.size(), MakeSpan("code-keyword-oper", name));
	return true;
}

bool VisualizerVisitor::VisitDefaultStmt(DefaultStmt* statement)
{
	std::string name = "default";
	mRewriter.ReplaceText(statement->getBeginLoc(), name.size(), MakeSpan("code-keyword-oper", name));
	return true;
}

bool VisualizerVisitor::VisitForStmt(ForStmt* statement)
{
	std::string name = "for";
	mRewriter.ReplaceText(statement->getBeginLoc(), name.size(), MakeSpan("code-keyword-oper", name));
	return true;
}

bool VisualizerVisitor::VisitWhileStmt(WhileStmt* statement)
{
	std::string name = "while";
	mRewriter.ReplaceText(statement->getBeginLoc(), name.size(), MakeSpan("code-keyword-oper", name));
	return true;
}

bool VisualizerVisitor::VisitDoStmt(DoStmt* statement)
{
	std::string name = "do";
	std::string aux = "while";
	mRewriter.ReplaceText(statement->getBeginLoc(), name.size(), MakeSpan("code-keyword-oper", name));
	mRewriter.ReplaceText(statement->getWhileLoc(), aux.size(), MakeSpan("code-keyword-oper", aux));
	return true;
}

bool VisualizerVisitor::VisitBreakStmt(BreakStmt* statement)
{
	std::string name = "break";
	mRewriter.ReplaceText(statement->getBeginLoc(), name.size(), MakeSpan("code-keyword-oper", name));
	return true;
}

bool VisualizerVisitor::VisitContinueStmt(ContinueStmt* statement)
{
	std::string name = "continue";
	mRewriter.ReplaceText(statement->getBeginLoc(), name.size(), MakeSpan("code-keyword-oper", name));
	return true;
}

bool VisualizerVisitor::VisitReturnStmt(ReturnStmt* statement)
{
	std::string name = "return";
	mRewriter.ReplaceText(statement->getBeginLoc(), name.size(), MakeSpan("code-keyword-oper", name));
	return true;
}

bool VisualizerVisitor::VisitIntegerLiteral(IntegerLiteral* literal)
{
	mRewriter.InsertText(literal->getLocation(), MakeOpenSpan("code-literal-num>"));
	mRewriter.InsertTextAfterToken(literal->getLocation(), std::format("</span>"));

	return true;
}

bool VisualizerVisitor::VisitFloatingLiteral(FloatingLiteral* literal)
{
	mRewriter.InsertText(literal->getLocation(), MakeOpenSpan("code-literal-num>"));
	mRewriter.InsertTextAfterToken(literal->getLocation(), std::format("</span>"));
	return true;
}

bool VisualizerVisitor::VisitStringLiteral(StringLiteral* literal)
{
	for (size_t i = 0; i != literal->getNumConcatenated(); ++i)
	{
		mRewriter.InsertText(literal->getStrTokenLoc(i), MakeOpenSpan("code-literal-str>"));
		mRewriter.InsertTextAfterToken(literal->getStrTokenLoc(i), std::format("</span>"));
	}
	return true;
}
