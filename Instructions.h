/*      Saphyr, a C++ style compiler using LLVM
        Copyright (C) 2012, Justin Madru (justin.jdm64@gmail.com)

        This program is free software: you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation, either version 3 of the License, or
        (at your option) any later version.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef __INSTRUCTIONS_H__
#define __INSTRUCTIONS_H__

#include "AST.h"
#include "CodeContext.h"

typedef CmpInst::Predicate Predicate;
typedef Instruction::BinaryOps BinaryOps;

class Inst
{
	static BinaryOps getOperator(int oper, SType* type, CodeContext& context);

	static Predicate getPredicate(int oper, SType* type, CodeContext& context);

	static void CastUp(RValue& lhs, RValue& rhs, CodeContext& context);

	static void CastMatch(RValue& lhs, RValue& rhs, CodeContext& context);

public:
	static bool isComplexExp(NodeType type);

	static void CastMatch(RValue& value, SType* type, CodeContext& context);

	static RValue BinaryOp(int type, RValue lhs, RValue rhs, CodeContext& context);

	static RValue Branch(BasicBlock* trueBlock, BasicBlock* falseBlock, NExpression* condExp, CodeContext& context);

	static RValue Cmp(int type, RValue lhs, RValue rhs, CodeContext& context);
};

#endif