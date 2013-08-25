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
#include "Instructions.h"
#include "parserbase.h"

void Inst::CastUp(RValue& lhs, RValue& rhs, CodeContext& context)
{
	auto ltype = lhs.stype();
	auto rtype = rhs.stype();

	if (ltype->matches(rtype)) {
		return;
	} else if (ltype->isArray() || rtype->isArray()) {
		context.addError("can not cast array types");
		return;
	}

	Value* val;
	if (ltype->isFloating()) {
		if (rtype->isFloating()) {
			if (ltype->isDouble()) {
				val = CastInst::CreateFPCast(rhs, *ltype, "", context);
				rhs = RValue(val, ltype);
			} else {
				val = CastInst::CreateFPCast(lhs, *rtype, "", context);
				lhs = RValue(val, rtype);
			}
		} else {
			val = new SIToFPInst(rhs, *ltype, "", context);
			rhs = RValue(val, ltype);
		}
	} else if (rtype->isFloating()) {
		if (ltype->isFloating()) {
			if (rtype->isDouble()) {
				val = CastInst::CreateFPCast(lhs, *rtype, "", context);
				lhs = RValue(val, rtype);
			} else {
				val = CastInst::CreateFPCast(rhs, *ltype, "", context);
				rhs = RValue(val, ltype);
			}
		} else {
			val = new SIToFPInst(lhs, *rtype, "", context);
			lhs = RValue(val, rtype);
		}
	} else {
		if (rtype->intSize() > ltype->intSize()) {
			val = CastInst::CreateIntegerCast(lhs, *rtype, true, "", context);
			lhs = RValue(val, rtype);
		} else if (rtype->intSize() < ltype->intSize()) {
			val = CastInst::CreateIntegerCast(rhs, *ltype, true, "", context);
			rhs = RValue(val, ltype);
		}
	}
}

void Inst::CastMatch(RValue& value, SType* type, CodeContext& context)
{
	auto valueType = value.stype();
	if (type->matches(valueType)) {
		return;
	} else if (type->isArray() || valueType->isArray()) {
		context.addError("can not cast array types");
		return;
	}

	Value* val;
	if (type->isFloating()) {
		if (valueType->isFloating())
			val = CastInst::CreateFPCast(value, *type, "", context);
		else
			val = new SIToFPInst(value, *type, "", context);
	} else if (type->isBool()) {
		// cast to bool is value != 0
		auto pred = getPredicate(ParserBase::TT_NEQ, valueType, context);
		auto op = valueType->isFloating()? Instruction::FCmp : Instruction::ICmp;
		val = CmpInst::Create(op, pred, value, RValue::getZero(valueType), "", context);
	} else if (valueType->isFloating()) {
		val = new FPToSIInst(value, *type, "", context);
	} else {
		val = CastInst::CreateIntegerCast(value, *type, true, "", context);
	}
	value = RValue(val, type);
}

BinaryOps Inst::getOperator(int oper, SType* type, CodeContext& context)
{
	if (type->isArray()) {
		context.addError("can not perform operation on entire array");
		return Instruction::Add;
	}
	switch (oper) {
	case '*':
		return type->isFloating()? Instruction::FMul : Instruction::Mul;
	case '/':
		return type->isFloating()? Instruction::FDiv : Instruction::SDiv;
	case '%':
		return type->isFloating()? Instruction::FRem : Instruction::SRem;
	case '+':
	case ParserBase::TT_INC:
		return type->isFloating()? Instruction::FAdd : Instruction::Add;
	case '-':
	case ParserBase::TT_DEC:
		return type->isFloating()? Instruction::FSub : Instruction::Sub;
	case ParserBase::TT_LSHIFT:
		if (type->isFloating())
			context.addError("shift operator invalid for float types");
		return Instruction::Shl;
	case ParserBase::TT_RSHIFT:
		if (type->isFloating())
			context.addError("shift operator invalid for float types");
		return Instruction::LShr;
	case '&':
		if (type->isFloating())
			context.addError("AND operator invalid for float types");
		return Instruction::And;
	case '|':
		if (type->isFloating())
			context.addError("OR operator invalid for float types");
		return Instruction::Or;
	case '^':
		if (type->isFloating())
			context.addError("XOR operator invalid for float types");
		return Instruction::Xor;
	default:
		context.addError("unrecognized operator " + to_string(oper));
		return Instruction::Add;
	}
}

Predicate Inst::getPredicate(int oper, SType* type, CodeContext& context)
{
	switch (oper) {
	case '<':
		return type->isFloating()? Predicate::FCMP_OLT : Predicate::ICMP_SLT;
	case '>':
		return type->isFloating()? Predicate::FCMP_OGT : Predicate::ICMP_SGT;
	case ParserBase::TT_LEQ:
		return type->isFloating()? Predicate::FCMP_OLE : Predicate::ICMP_SLE;
	case ParserBase::TT_GEQ:
		return type->isFloating()? Predicate::FCMP_OGE : Predicate::ICMP_SGE;
	case ParserBase::TT_NEQ:
		return type->isFloating()? Predicate::FCMP_ONE : Predicate::ICMP_NE;
	case ParserBase::TT_EQ:
		return type->isFloating()? Predicate::FCMP_OEQ : Predicate::ICMP_EQ;
	default:
		context.addError("unrecognized predicate " + to_string(oper));
		return Predicate::ICMP_EQ;
	}
}

bool Inst::isComplexExp(NodeType type)
{
	switch (type) {
	case NodeType::IntConst:
	case NodeType::FloatConst:
	case NodeType::Variable:
	case NodeType::ArrayVariable:
		return false;
	default:
		return true;
	}
}

RValue Inst::BinaryOp(int type, RValue lhs, RValue rhs, CodeContext& context)
{
	auto llvmOp = getOperator(type, lhs.stype(), context);
	auto llvmVal = BinaryOperator::Create(llvmOp, lhs, rhs, "", context);
	return RValue(llvmVal, lhs.stype());
}

RValue Inst::Branch(BasicBlock* trueBlock, BasicBlock* falseBlock, NExpression* condExp, CodeContext& context)
{
	auto condValue = condExp? condExp->genValue(context) : RValue::getOne(SType::getBool(context));
	CastMatch(condValue, SType::getBool(context), context);
	BranchInst::Create(trueBlock, falseBlock, condValue, context);
	return condValue;
}

RValue Inst::Cmp(int type, RValue lhs, RValue rhs, CodeContext& context)
{
	auto pred = getPredicate(type, lhs.stype(), context);
	auto op = rhs.stype()->isFloating()? Instruction::FCmp : Instruction::ICmp;
	auto cmp = CmpInst::Create(op, pred, lhs, rhs, "", context);
	return RValue(cmp, SType::getBool(context));
}
