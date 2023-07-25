#include "mthpch.h"

#include "Core/Lexer.h"

#include "Utility/Utils.h"

Lexer::Lexer() : mTokens({}), mOperationIndexes({}) {}

// REFACTOR : Marked Lexer::GenerateTokens for refactor.
void Lexer::GenerateTokens(String equation)
{
	mTokens = {};
	mOperationIndexes = {};

	Mathematica::RemoveQuotes(equation);
	Vector<String> separetedEquation = Mathematica::SeparateString(equation);

	uint32 parenthesesCount = 0;
	bool invertSign = false;
	bool skipMarker = false;

	auto isAnOperation = [](const char c) -> bool
	{
		const char operations[] = { '+', '-', '*', '/' };

		for (int32 i = 0; i < 4; i++)
		{
			if (c == operations[i])
			{
				return true;
			}
		}

		return false;
	};

	auto isParenthesis = [](const char c) -> const char*
	{
		const char parentheses[] = { '(', '[', '{', ')', ']', '}' };

		for (int32 i = 0; i < 6; i++)
		{
			if (c == parentheses[i])
			{
				return i < 3 ? "Start" : "End";
			}
		}

		return "No";
	};

	// REFACTOR : The first half of the for loop might be deleted in the future, as well as string separation.
	// REFACTOR : Instead, it might be convenient to just ignore the spaces.
	for (auto substring : separetedEquation)
	{
		// TODO (late) : Change the for loop to match literal expressions.
		// Hint: if size is 1 then it may be an operation. 
		if (substring.size() == 1)
		{
			if (isAnOperation(substring.front())) // <--- Check if it is an operation
			{
				// Check if the previous token was a function or if the tokens are empty.
				// then, check if this operation is a "+" or a "-"
				// if those checks are valid, then add a flag to represent the sign of the number.
				if (mTokens.size() == 0 || mTokens.back().type == ELexiconTokenType::BinaryFunction)
				{
					switch(substring.front())
					{
						case '+': break;
						case '-': invertSign = !invertSign; break;
						default: MTH_ASSERT(false, "SyntaxError: Invalid token identified!"); break;
					}

					skipMarker = true;
				}

				// Add the operation to the back of the tokens.
				if(!skipMarker)
				{
					mTokens.emplace_back(substring, ELexiconTokenType::BinaryFunction);

					// Set the priority for the operation index.
					if (substring.front() == '+' || substring.front() == '-')
					{
						mOperationIndexes[parenthesesCount][EPriority::Low].push_back((int32)(mTokens.size() - 1));
						
					}
					else
					{
						mOperationIndexes[parenthesesCount][EPriority::Medium].push_back((int32)(mTokens.size() - 1));
					}
				}
				else
				{
					skipMarker = false;
				}

				continue;
			}
			else if (String result = isParenthesis(substring.front()); result != "No") // <--- Check if it is a parenthesis
			{
				if (result == "Start")
				{
					// Check if the previous token was a number or a parenthesis.
					if (!mTokens.empty() && (mTokens.back().type == ELexiconTokenType::Number || mTokens.back().type == ELexiconTokenType::WrapperEnd))
					{
						// If so, create a multiplication token.
						mTokens.emplace_back("*", ELexiconTokenType::BinaryFunction);
						mOperationIndexes[parenthesesCount][EPriority::Medium].push_back((int32)(mTokens.size() - 1));
					}
					mTokens.emplace_back("#", ELexiconTokenType::WrapperStart);
					parenthesesCount++;
					
					// Initialize scope counter to send information to the parser.
					if (mScopeCounter.find(parenthesesCount) != mScopeCounter.end())
					{
						mScopeCounter[parenthesesCount].first++;
						mScopeCounter[parenthesesCount].second.emplace_back(mTokens.size() - 1, (uint32)0);
					}
					else
					{
						Vector<Pair<uint32, uint32>> parLocation;
						parLocation.emplace_back(mTokens.size() - 1, (uint32)0);
						Pair<uint32, Vector<Pair<uint32, uint32>>> scopeCount = {(uint32)1, parLocation};
						mScopeCounter[parenthesesCount] = scopeCount;
					}
				}
				else
				{
					mTokens.emplace_back("#", ELexiconTokenType::WrapperEnd);
					mScopeCounter[parenthesesCount].second[mScopeCounter[parenthesesCount].first - 1].second = mTokens.size() - 1;
					parenthesesCount--;
				}

				continue;
			}
		}

		// Check if a substring contains both numbers and operations.
		bool bIsNumber = true;
		String currentSubsubstring = "";
		for (auto currentChar : substring)
		{
			if (!(currentChar < 48 || currentChar > 57) || currentChar == '.') // <--- Check if it is a number
			{
				// Check if the flag bIsNumber is false. If so, throw a SyntaxError.
				// TODO : Build class SyntaxError.
				if (!bIsNumber) // <-- This statement should never be called. However I should check that, I'll leave it here for now.
				{
					MTH_ASSERT(bIsNumber, "SyntaxError: An unknown token has been found!");
					//mTokens.emplace_back(currentSubsubstring, ELexiconTokenType::Unknown);
					//currentSubsubstring = "";
					//bIsNumber = true;
				}
				// Check if the previous token was a closing parenthesis.
				if (!mTokens.empty() && mTokens.back().type == ELexiconTokenType::WrapperEnd)
				{
					// If so, create a multiplication token.
					mTokens.emplace_back("*", ELexiconTokenType::BinaryFunction);
				}

				currentSubsubstring += currentChar;
			}
			else if(isAnOperation(currentChar)) // <--- Check if it is an operation
			{
				// Check if currentSubsubstring is not empty. If so, it means it is a number, so append to mTokens.
				if (!currentSubsubstring.empty())
				{
					mTokens.emplace_back(StringifyNumberToken(currentSubsubstring, invertSign), ELexiconTokenType::Number);
					invertSign = false;
				}
				else
				{
					// If it is empty, It might be necessary to generate a zero token to represent the sign.
					// This, again, it is only possible if the previous token was an operation or the mTokens array is empty.
					// Also, the current token must be a '+' or a '-'.
					if (mTokens.size() == 0 || mTokens.back().type == ELexiconTokenType::BinaryFunction)
					{
						switch(currentChar)
						{
							case '+': break;
							case '-': invertSign = !invertSign; break;
							default: MTH_ASSERT(false, "SyntaxError: Invalid token identified!"); break;
						}

						skipMarker = true;
					}
				}

				// Append the operation to mTokens.
				if(!skipMarker)
				{
					mTokens.emplace_back(String(1, currentChar), ELexiconTokenType::BinaryFunction);

					// Set the priority of such operation.
					if (currentChar == '+' || currentChar == '-')
					{
						mOperationIndexes[parenthesesCount][EPriority::Low].push_back((int32)(mTokens.size() - 1));
					}
					else
					{
						mOperationIndexes[parenthesesCount][EPriority::Medium].push_back((int32)(mTokens.size() - 1));
					}
				}
				else
				{
					skipMarker = false;
				}

				// Reset the currentSubsubstring, to allow a new number to be stored.
				currentSubsubstring = "";
			}
			else if (String result = isParenthesis(currentChar); result != "No") // <--- Check if it a parenthesis
			{
				if (!currentSubsubstring.empty())
				{
					mTokens.emplace_back(StringifyNumberToken(currentSubsubstring, invertSign), ELexiconTokenType::Number);
					invertSign = false;
				}
				if (result == "Start")
				{	
					// Check if the previous token was a number or a parenthesis.
					if (!mTokens.empty() && (mTokens.back().type == ELexiconTokenType::Number || mTokens.back().type == ELexiconTokenType::WrapperEnd))
					{
						// If so, create a multiplication token.
						mTokens.emplace_back("*", ELexiconTokenType::BinaryFunction);
						mOperationIndexes[parenthesesCount][EPriority::Medium].push_back((int32)(mTokens.size() - 1));
					}
					mTokens.emplace_back("#", ELexiconTokenType::WrapperStart);
					parenthesesCount++;

					// Initialize scope counter to send information to the parser.
					if (mScopeCounter.find(parenthesesCount) != mScopeCounter.end())
					{
						mScopeCounter[parenthesesCount].first++;
						mScopeCounter[parenthesesCount].second.emplace_back(mTokens.size() - 1, (uint32)0);
					}
					else
					{
						Vector<Pair<uint32, uint32>> parLocation;
						parLocation.emplace_back(mTokens.size() - 1, (uint32)0);
						Pair<uint32, Vector<Pair<uint32, uint32>>> scopeCount = { (uint32)1, parLocation };
						mScopeCounter[parenthesesCount] = scopeCount;
					}
				}
				else
				{
					mTokens.emplace_back("#", ELexiconTokenType::WrapperEnd);
					mScopeCounter[parenthesesCount].second[mScopeCounter[parenthesesCount].first - 1].second = mTokens.size() - 1;
					parenthesesCount--;
				}
				// --solve "(1+2)(2+1)"

				currentSubsubstring = "";
			}
			else
			{
				// This is an unknown token! We should throw here!
				bIsNumber = false;
				MTH_ASSERT(bIsNumber, "SyntaxError: An unknown token has been found!");

				if (!currentSubsubstring.empty() && bIsNumber)
				{
					mTokens.emplace_back(StringifyNumberToken(currentSubsubstring, invertSign), ELexiconTokenType::Number);
					currentSubsubstring = "";
				}
				currentSubsubstring += currentChar;
			}
		}

		if (!currentSubsubstring.empty())
		{
			if (bIsNumber)
			{
				mTokens.emplace_back(StringifyNumberToken(currentSubsubstring, invertSign), ELexiconTokenType::Number);
				invertSign = false;
			}
			else
			{
				MTH_ASSERT(false, "Syntax error: An unknown token has been found!");
				//mTokens.emplace_back(currentSubsubstring, ELexiconTokenType::Unknown);
			}
		}
	}

	MTH_ASSERT(parenthesesCount == 0, "Syntax error: parentheses mismatch found!");
}

String Lexer::StringifyNumberToken(String string, bool invertSign)
{
	bool bSkip = false;
	String number = "";
	for (auto digit : string)
	{
		if (digit == '0' && !bSkip)
		{
			continue;
		}
		else
		{
			if (!bSkip)
			{
				bSkip = true;
			}
			number += digit;
		}
	}


	number = number.length() > 0 ? number : "0";
	number = number != "0" && invertSign ? "-" + number : number; 

	return number;
}

// REFACTOR : Change C-like casts into C++-like casts.
