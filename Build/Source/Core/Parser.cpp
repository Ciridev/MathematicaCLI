#include "mthpch.h"

#include "Core/Number.h"
#include "Core/Parser.h"

#include "Utility/Utils.h"

MParser::MParser()
{
	mNodes = {};
	mScopedIndex = -1;
}

void MParser::InitParser(const MVector<MLexiconToken>& tokens, const MMap<uint32, MHashMap<EPriority, MVector<uint32>>>& opIndexes, const MHashMap<uint32, MPair<uint32, MVector<MPair<uint32, uint32>>>>& scopeCounter)
{
	GenerateNodes(tokens);
	mOperationIndexes = opIndexes;
	mScopeCounter = scopeCounter;
}

void MParser::GenerateWrappedNodes(const MVector<uint32>& indexes)
{
	for (auto index : indexes)
	{
		MRef<MMathNode> wrappedNode = mNodes[index];

		// Check if previous and next node are marked to be ignored and get the indexes for the left and right nodes.
		uint32 leftCounter = 1;
		uint32 rightCounter = 1;
		uint32 copyIndex = index;
		while (mNodes[--copyIndex]->type == EMathNodeType::None) leftCounter++;
		copyIndex = index;
		while (mNodes[++copyIndex]->type == EMathNodeType::None) rightCounter++;

		// Instantiate a reference for these nodes
		MRef<MMathNode> leftNode = mNodes[index - leftCounter];
		MRef<MMathNode> rightNode = mNodes[index + rightCounter];

		// Make sure that the user sent a proper input.
		// Then, unwrap nodes and make links between parents and children.
		MTH_ASSERT(leftNode->type == EMathNodeType::Number || leftNode->type == EMathNodeType::Wrapper,
			"Parser error: tree generation cannot be completed, [leftNode] was not idoneous.");
		MTH_ASSERT(rightNode->type == EMathNodeType::Number || rightNode->type == EMathNodeType::Wrapper,
			"Parser error: tree generation cannot be completed, [rightNode] was not idoneous.");

		leftNode = (leftNode->type == EMathNodeType::Wrapper) ? leftNode->children.back() : leftNode;
		rightNode = (rightNode->type == EMathNodeType::Wrapper) ? rightNode->children.back() : rightNode;

		wrappedNode->children.push_back(leftNode);
		wrappedNode->children.push_back(rightNode);
		leftNode->parent = wrappedNode;
		rightNode->parent = wrappedNode;

		// Create a new wrapper node and append the wrapped tree. 
		// Then create new left and right nodes, and mark them to be ignored for the next calls.
		mNodes[index] = Mathematica::MakeRef<MMathNode>();
		mNodes[index]->type = EMathNodeType::Wrapper;
		mNodes[index]->children.push_back(wrappedNode);
		mNodes[index - leftCounter] = Mathematica::MakeRef<MMathNode>();
		mNodes[index + rightCounter] = Mathematica::MakeRef<MMathNode>();
		mNodes[index - leftCounter]->type = EMathNodeType::None;
		mNodes[index + rightCounter]->type = EMathNodeType::None;

		mScopedIndex = Mathematica::Min(mScopedIndex, index);
	}
}

void MParser::GenerateNodes(const MVector<MLexiconToken>& tokens)
{
	for (auto token : tokens)
	{
		MRef<MMathNode> currentNode = Mathematica::MakeRef<MMathNode>();

		// Create node
		switch (token.type)
		{
		case ELexiconTokenType::Number:
			currentNode->tokenData = MNumber(token.data);
			currentNode->type = EMathNodeType::Number;
			break;
		case ELexiconTokenType::BinaryFunction:
			currentNode->tokenData = Mathematica::GetBinaryFunctionFromRawData(token.data);
			currentNode->type = EMathNodeType::BinaryFunction;
			break;
		case ELexiconTokenType::WrapperStart:
			currentNode->type = EMathNodeType::WrapStart;
			break;
		case ELexiconTokenType::WrapperEnd:
			currentNode->type = EMathNodeType::WrapEnd;
			break;
		default:
			MTH_ASSERT(false, "Parser error: Unkown token detected!");
			break;
		}

		mNodes.push_back(currentNode);
	}
}

MRef<MMathNode> MParser::GenerateTree()
{
	for (auto it = mOperationIndexes.rbegin(); it != mOperationIndexes.rend(); it++)
	{
		auto scopeIndex = it->first;
		auto scopeData = it->second;

		auto highIndexes = scopeData[EPriority::High];
		auto normalIndexes = scopeData[EPriority::Normal];
		auto lowIndexes = scopeData[EPriority::Low];

		GenerateWrappedNodes(highIndexes);
		GenerateWrappedNodes(normalIndexes);
		GenerateWrappedNodes(lowIndexes);

		auto dummy = mScopeCounter[scopeIndex].second;

		for (auto [first, second] : dummy)
		{
			mNodes[first]->type = EMathNodeType::None;
			mNodes[second]->type = EMathNodeType::None;
		}
	}

	for (auto node : mNodes)
	{
		if (node->type == EMathNodeType::Wrapper)
		{
			mTree = node->children.back();
			return mTree;
		}
	}

	return nullptr;
}
