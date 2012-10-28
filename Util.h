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
#ifndef __UTIL__
#define __UTIL__

#include <llvm/Value.h>
#include <llvm/Type.h>
#include <llvm/Instructions.h>
#include "Constants.h"
#include "CodeContext.h"

void typeCastMatch(Value*& value, Type* type, CodeContext& context);

void typeCastUp(Value*& lhs, Value*& rhs, CodeContext& context);

Instruction::BinaryOps getOperator(int oper, Type* type, CodeContext& context);

ICmpInst::Predicate getPredicate(int oper, Type* type, CodeContext& context);

bool isComplexExp(NodeType type);

#endif
