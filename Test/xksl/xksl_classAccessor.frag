/****************************************************************************************
Parse shader with class accessor: this, base, className
ClassName has to be known, otherwise the expression will be set as unresolved.

*****************************************************************************************/

shader S01
{
	int a;
};

shader S03: S01, S02
{
	int c;
	
	int f()
	{
		int res = 0;
		res += S01.a;
		res += S02.b;
		res += S03.c;
		
		res += this.a;
		res += this.b;
		res += this.c;
		
		res += base.a;
		//res += base.b;  //Error, b dos not exists in S01
		
		return res;
	}
};

shader S02
{
	int b;
};