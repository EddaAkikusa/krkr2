local a=10;
local b = ::krkr.a;
local c = ::krkr.b;
::krkr.c = 100;

test <- "�Ă��Ƃ̃e�L�X�g";


class Foo {

	//constructor
	constructor()
	{
		testy = ["stuff",1,2,3];
	}

	//member function
	function print()
	{
		foreach(i,val in testy)
		{
			::print("idx = "+i+" = "+val+" \n");
		}
	}
	//property
	testy = null;
	data="testdata";
};

function hoge()
{
	print("�t�@���N�V�����Ăяo��!");
}

// �t�@�C���Ăяo���e�X�g
dofile("test2.nut");

// �t�@�C���������݃e�X�g
local a = file("output", "w");
a.writen(10, 'l');
a = null;

// �g���g�����̃I�u�W�F�N�g����
local layer2 = ::krkr.MyLayer(::krkr.win, ::krkr.base);
layer2.setSize(100,100);
layer2.setPos(100,100);
layer2.fillRect(0,0,100,100,0xff00ff00);
layer2.visible = true;
return layer2;
