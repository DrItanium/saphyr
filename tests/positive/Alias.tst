
alias Ptr1 = @int;
alias MyPtr = Ptr1;

MyPtr glob = null;

void bla(MyPtr a)
{
	auto b = a@;
	Ptr1 c = null;
}

========

@glob = global i32* null

define void @bla(i32* %a) {
  %1 = alloca i32*
  store i32* %a, i32** %1
  %2 = load i32** %1
  %3 = load i32* %2
  %b = alloca i32
  store i32 %3, i32* %b
  %c = alloca i32*
  store i32* null, i32** %c
  ret void
}
