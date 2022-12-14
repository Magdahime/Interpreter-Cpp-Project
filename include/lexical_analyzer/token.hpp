#pragma once
#include <variant>
#include <cstdint>
#include <string>
#include "matrix.hpp"
#include "source.hpp"

using TokenVariant = std::variant<std::monostate, int64_t, double, std::string, Matrix>;

class Token
{
public:
    enum class TokenType
    {
        MatrixToken,
        IntegerToken,
        TextToken,
        DoubleToken,
        AdditiveOperatorToken,
        MultiplicativeOperatorToken,
        RelationalOperatorToken,
        LogicalOperatorToken,
        AssignmentOperatorToken,
        IfToken,
        OtherwiseToken,
        LoopToken,
        AsLongAsToken,
        FunctionToken,
        ConditionToken,
        CaseToken,
        DefaultToken,
        IdentifierToken,
        OpenRoundBracketToken,
        CloseRoundBracketToken,
        OpenSquareBracketToken,
        CloseSquareBracketToken,
        ColonToken,
        OpenBlockToken,
        CloseBlockToken,
        CommaToken,
        PointToken,
        VoidToken,
        ContinueToken,
        BreakToken,
        TrueToken,
        FalseToken,
        CommentToken,
        EndOfFileToken,
        UnindentifiedToken,
        NextLineToken,
        AndToken,
        OrToken,
        NotToken,
        StringLiteralToken,
        IntegerLiteralToken,
        DoubleLiteralToken,
        DetToken,
        TransToken,
        InvToken,
        ReturnToken,
    };

    enum class TokenSubtype
    {
        PlusToken,
        MinusToken,
        GreaterOrEqualToken,
        LessOrEqualToken,
        LessToken,
        GreaterToken,
        EqualToken,
        NotEqualToken,
        DivisionToken,
        MultiplicationToken,
    };

    Token(TokenType type, TokenVariant value,
         NextCharacter& firstCharacter)
        : type(type), value(value),
        characterPosition(firstCharacter.characterPosition),
        absolutePosition(firstCharacter.absolutePosition), linePosition(firstCharacter.linePosition) {}
    Token(TokenType type, TokenSubtype subtype, TokenVariant value,
        NextCharacter& firstCharacter)
        : type(type), subtype(subtype), value(value),
        characterPosition(firstCharacter.characterPosition),
        absolutePosition(firstCharacter.absolutePosition), linePosition(firstCharacter.linePosition) {}
    Token(TokenType type) : type(type) {}
    Token(TokenType type, TokenVariant value) : type(type), value(value) {}
    TokenType getType() const { return type; }
    TokenSubtype getSubtype() const { return subtype; }
    TokenVariant getValue() const{return value;}
    uint64_t getCharacterPosition() const { return characterPosition; }
    uint64_t getAbsolutePosition() const { return absolutePosition; }
    uint64_t getLinePosition() const { return linePosition; }

private:
    TokenType type;
    TokenSubtype subtype;
    TokenVariant value;
    uint64_t characterPosition;
    uint64_t absolutePosition;
    uint64_t linePosition;

    friend bool operator==(Token const &lhs, Token const &rhs)
    {
        return lhs.type == rhs.type && lhs.value == rhs.value;
    };

    friend bool operator!=(Token const &lhs, Token const &rhs)
    {
        return  !operator==(rhs, lhs);
    };
};
