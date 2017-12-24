#include <iostream>
#include <string>
#include <fstream>
#include <cctype>
#include <map>
#include <vector>
#include <set>
#include <iomanip>
#include <sstream>//strinfstream and istringstream
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <algorithm>

class symbol
{
public:
    symbol(std::string _name = "", 
	       int _addr = -1, int _mod_num = -1,
		   bool _defined = false, bool _never_used = true)
		{
			name = _name;
			addr = _addr;
			mod_num = _mod_num;
			defined = _defined;
			never_used =  _never_used;
		};
    std::string name;
    int addr;
    int mod_num;
    bool defined;
    bool never_used;
};

//global variables:
std::vector<std::string> sym_set;
//using vector since the order really matters!
std::map<std::string, symbol> sym_map;//the map of defined symbols

//must support indexing!!!
std::vector<int> module_size(10,0);
std::vector<int> module_staddr(10,0);
std::vector<std::string> use_list;
std::vector<bool> used;

//the parser class:
class parser
{
	public:
		parser(std::istream & _infile):
			infile(_infile)
		{
			line_num = 0;
			char_offset = 1;
			add_offset = 0;
		}

		int readNumDef();
		int read_offset();
		void readDefList(int &module_num, int module_addr, int pass);
		int readNumUse();
		void readUseList(int pass);
		std::string read_symbol();
		char read_instype();
		int read_inst();
		int readNumInst();
		void readInstList(int &module_addr, int module_num, int pass);
		int line_num, char_offset, add_offset;

	private:
  		std::istream &infile;
		std::stringstream cur_line, cur_str;
		int readNum(int flag);
		void get_nextline();
		void get_nextstr();
		void print_inst(char instype, int inst, int module_num, int instaddr);
		void _parseerror(int errcode, int linenum, int lineoffset);
};

//if parse error, abort processing!
//ref: http://www.cplusplus.com/reference/stdexcept/runtime_error/
void parser::_parseerror(int errcode, int linenum, int lineoffset)
{
	//error message:
	static char* errstr[] = {
		"NUM_EXPECTED",
		"SYM_EXPECTED",
		"ADDR_EXPECTED",
		"SYM_TOO_LONG",
		"TO_MANY_DEF_IN_MODULE",
		"TO_MANY_USE_IN_MODULE",
		"TO_MANY_INSTR",
	};

	//follow the hint
	printf("Parse Error line %d offset %d: %s\n", linenum, lineoffset, errstr[errcode]);
	exit(1);
}

//since we could have a "\n" in a single list
//we better not give the dilimiter in getline()
//just store it in istringstream
//ref: http://stackoverflow.com/questions/18318980/taking-input-of-a-string-word-by-word
void parser::get_nextline()
{
	std::string line;
	cur_line.clear();
	//very important: if there is only one '\n'
	//don't increase the line_num;
	if(std::getline(infile,line))
	{
		cur_line << line;
		line_num++;
		char_offset = 1;
		//std::cout<<"行数"<<line_num<<"start from 1"<<std::endl;
	}
	return;
}

void parser::get_nextstr()
{
	//try to get the next str
	std::string word;
	cur_str.clear();
	char_offset += add_offset;
	while(infile)
	{
		char c = cur_line.peek();
		while(c == ' '||c == '\t'/*||c == '\n'*/)
		{
			char_offset++;
			cur_line.ignore(1);
			c = cur_line.peek();
		}
		//notice that tokens in a single list are probably not in the same line!
		if(!(cur_line >> word))
		{
			get_nextline();
			continue;
		}
		add_offset = word.length();
		cur_str << word;
		break;
	}
	return;
}

int parser::readNum(int flag)//flag dealing with edge case "\n" in the end;
{
	int i;
	get_nextstr();
	if(!infile.good()) return 0;
	if(!(cur_str >> i))
	{
		if(flag==1)
		{
			if(!infile.eof()) _parseerror(0, line_num, char_offset);//num expected
		}
		else _parseerror(0, line_num, char_offset);//num expected
	}
	return i;
}

int parser::readNumDef()
{
	int def_num;
	def_num = readNum(1);
	if(!infile.good()) return 0;
	//would it be possible that def_num<0???
	if(def_num < 0)
	{
		_parseerror(0, line_num, char_offset);//num expected
	}
	else if(def_num > 16)
	{
		_parseerror(4, line_num, char_offset);//too many def
	}
	return def_num;
}

int parser::readNumUse()
{
	int use_num;
	use_num = readNum(1);
	if(!infile.good()) return 0;
	if(use_num < 0)
	{
		_parseerror(0, line_num, char_offset);//num expected
	}
	else if(use_num > 16)
	{
		_parseerror(5, line_num, char_offset);//too many use
	}
	return use_num;
}

int parser::readNumInst()
{
	int inst_num;
	inst_num = readNum(1);
	if(!infile.good()) return 0;
	//if(inst_num > 512) _parseerror(6, line_num, char_offset);//total num_inst exceeds memory size (512)
	return inst_num;
}

//This is for def
std::string parser::read_symbol()
{
	std::string sym;
	get_nextstr();
	if(!(cur_str >> sym) ||
		sym.length() < 1 ||
		!isalpha(sym[0]))
	{
		_parseerror(1, line_num, char_offset);//Symbol Expected
	}
	//ref:http://www.cplusplus.com/reference/cctype/isalpha/
	else if(sym.length() > 16)
	{
		_parseerror(3, line_num, char_offset);//Symbol Name is too long
	}
	for(int i = 1; i < sym.length(); i++)
	{
		if(!isalnum(sym[i]))
		{
			_parseerror(1, line_num, char_offset);//Symbol Expected
		}
	}
	return sym;
}

//this is for def:
int parser::read_offset()
{
	int addr;
	addr = readNum(0);
	if(addr < 0)
	{
		_parseerror(0, line_num, char_offset);//Number expect
	}
	return addr;
}

void create_sym(std::string sym, int module_num, int absaddr)
{
	sym_set.push_back(sym);
	symbol ss;
	ss.name = sym;
	ss.addr = absaddr;
	ss.mod_num = module_num;
	ss.never_used = true;
	sym_map[sym] = ss;
}

void parser::readDefList(int &module_num, int module_addr, int pass)
{
	int def_num = readNumDef();
	if(!infile.good()) return;
	//std::cout<<"在read def zhehang:"<<line_num<<"def_num="<<def_num<<std::endl;
	if(pass==1)
	{
		module_num++;
		std::vector<int>::iterator it;
		it = module_staddr.begin();
		module_staddr.insert(it+module_num, module_addr);
	}
	if(def_num != 0)
	{
		for(int i = 0; i < def_num; i++)
		{
			std::string symbol;
			int offset;

			symbol = read_symbol();
			offset = read_offset();
			if(pass==1)
			{
				if(std::find(sym_set.begin(), sym_set.end(), symbol) != sym_set.end())
				{
					//std::cout<<"dsff"<<endl;
					sym_map[symbol].defined = true;
					//if a symbol has been already defined, using the first definition
					continue;
				}
				else
				{
					//sym_map[symbol].mod_num = module_num;
					int absaddr = module_addr + offset;
					create_sym(symbol, module_num, absaddr);
					//std::cout<<absaddr<<" "<<module_num<<" "<<sym_map[symbol].mod_num<<std::endl;
				}
			}
		}
	}
}

void parser::readUseList(int pass)
{
	int use_num = readNumUse();
	if(!infile.good()) return;
	//std::cout<<"在read use zhehang:"<<line_num<<"use_num="<<use_num<<std::endl;
	for(int i = 0; i < use_num; i++)
	{
		std::string symbol;
		symbol = read_symbol();
		if(pass==2)
		{
			sym_map[symbol].never_used = false;
			use_list.push_back(symbol);
			used.push_back(false);
		}
	}
}

//this is for instlist
char parser::read_instype()
{
	std::string inst;
	get_nextstr();
	if(!(cur_str >> inst) || inst.length() != 1)
	{
		_parseerror(2, line_num, char_offset);//Addressing Expected
	}
	else
	{
		char c = inst[0];
		switch(c)
		{
			case 'I':
			case 'A':
			case 'R':
			case 'E': break;
			default:  _parseerror(2, line_num, char_offset);//Addressing Expected
		}
	}
	return inst[0];
}

int parser::read_inst()
{
	int inst;
	inst = readNum(0);
	return inst;
}

void parser::readInstList(int &module_addr, int module_num, int pass)
{
	int inst_num = readNumInst();
	if(!infile.good()) return;

	//std::cout<<"在read inst zhehang:"<<line_num<<"inst_num="<<inst_num<<std::endl;
	//only check the parseerror in the first pass!
	if(pass==1)
	{
		if(module_addr + inst_num >= 512)
		_parseerror(6, line_num, char_offset);//total num of inst exceeds memory size (512)
	}

	for(int i = 0; i < inst_num; i++)
	{
		char instype;
		int inst;
		instype = read_instype();
		inst = read_inst();
		if(pass==2)
		{
			print_inst(instype, inst, module_num, (module_addr + i));
		}
	}

	if(pass==1)
	{
		std::vector<int>::iterator it;
		it = module_size.begin();
		module_size.insert(it+module_num, inst_num);
		module_addr += inst_num;
	}
}

void parser::print_inst(char instype, int inst, int module_num, int instaddr)
{
	std::string err;
	//std::string opcode_err;
	int opcode = inst / 1000;
	int oprand = inst % 1000;
	int res;
	switch(instype)
	{
		case 'I':
			res = inst;
			if(inst>9999)
			{
				res = 9999;
				err = "Error: Illegal immediate value; treated as 9999";//rule 10
			}
			break;

		case 'R':
			if(oprand > module_size[module_num])
			{
				oprand = 0;
				err = "Error: Relative address exceeds module size; zero used";//rule 9
			}
			res = opcode*1000 + oprand + module_staddr[module_num];
			break;

		case 'A':
			if(oprand > 512)
			{
				oprand = 0;
				err = "Error: Absolute address exceeds machine size; zero used";//rule 8
			}
			res = opcode*1000 + oprand;
			break;

		case 'E':
			//start from 0!
			if(oprand >= use_list.size())
			{
				res = inst;
				if(inst>9999)
				{
					res = 9999;//rule 10
				}//???
				err = "Error: External address exceeds length of uselist; treated as immediate";//rule 6
			}
			else
			{
				//remember to mark the corresponding sym used!!!
				used[oprand] = true;
				std::string sym = use_list[oprand];
				if(std::find(sym_set.begin(), sym_set.end(), sym) == sym_set.end())
				{
					err = "Error: " + sym + " is not defined; zero used";//rule 3
					res = opcode*1000;
				}
				else
				{
					oprand = sym_map[sym].addr;
					res = opcode*1000 + oprand;
				}
				//std::cout<<oprand<<"marked true"<<std::endl;
			}
			break;
	}
	if(res>9999)
	{
		res = 9999;
		err = "Error: Illegal opcode; treated as 9999";
	}
	//ref:http://www.c4learn.com/cplusplus/cpp-setw-setting-field-width/
	std::cout << std::setfill('0') << std::setw(3) << instaddr << ": "
				<< std::setfill('0') << std::setw(4) << res;

	if(!err.empty())
		std::cout << ' ' << err;
	std::cout << std::endl;
}

//first pass:
int first(std::istream &infile)//std::infile
{
	int pass = 1, module_num = 0, module_addr = 0;
	parser p1(infile);
	//read three lists:
	while(infile)
	{
		// readDefList
		p1.readDefList(module_num, module_addr, 1);
		//notice that only when readDefList() succeed we increase module_num;
		// readUseList
		p1.readUseList(1);
		// readInstList
		p1.readInstList(module_addr,module_num,1);
	}
	//return the total num of module!
	return module_num;
}

//second pass:
void second(std::istream &infile, int module_total)
{
	int pass = 2, module_num = 1;
	parser p2(infile);

	//ref:http://stackoverflow.com/questions/26281979/c-loop-through-map
	for(; module_num <= module_total; module_num++)
	{
		int mod_addr = module_staddr[module_num];
		//readDefList
		p2.readDefList(module_num, mod_addr, 2);
		//notice that only when readDefList() succeed we increase module_num
		//before readUseList, clear the use_list for this module
		use_list.clear();
		used.clear();
		// readUseList
		p2.readUseList(2);
		// readInstList
		p2.readInstList(mod_addr, module_num, 2);

		for(int i = 0; i < use_list.size(); i++)
		{
			if(!used[i])
			{
				std::cout << "Warning: Module " << module_num << ": " << use_list[i]
						<<" appeared in the uselist but was not actually used" << std::endl;//rule 7
			}
		}
	}
}

int main(int argc, char **argv)
{
	std::ifstream infile;
	infile.open(argv[1]);

	//first pass
	int module_total = first(infile);

	//after pass one, print:
	//check for rule 5
	for(std::vector<std::string>::iterator it = sym_set.begin(); it != sym_set.end(); it++)
	{
		//if we want to use operator[] to overwrite the value already exist:
		std::string sym1(*it);
		//follow the hint:
		int modnum = sym_map[sym1].mod_num;
		int mod_size = module_size[modnum];
		int mod_st = module_staddr[modnum];
		int sym_ad = sym_map[sym1].addr;
		int sym_offset = sym_ad - mod_st;

		if(sym_offset >= mod_size)
		{
	  		std::cout << "Warning: Module " << modnum << ": " << sym1
				<< " too big " << sym_offset << " (max=" << (mod_size-1)
				<< ") assume zero relative" << std::endl;
			sym_map[sym1].addr = mod_st + 0;
		}
	}

	std::cout << "Symbol Table"<<std::endl;

	//check for rule 2
	for(std::vector<std::string>::iterator it = sym_set.begin(); it != sym_set.end(); it++)
	{
		std::string sym2(*it);
		std::cout << sym2 << '=' << sym_map[sym2].addr;
		if(sym_map[sym2].defined)
		{
			std::cout << " Error: This variable is multiple times defined; first value used" << std::endl;//rule 2
		}
		std::cout << '\n';
	}

	std::cout << '\n';
	infile.close();
	infile.open(argv[1]);

	std::cout << "Memory Map" << std::endl;

	//second pass
	second(infile, module_total);
	std::cout << '\n';

	for(std::vector<std::string>::iterator it = sym_set.begin(); it != sym_set.end(); it++)
	{
		std::string sym3(*it);
		if(sym_map[sym3].never_used)
		{
			std::cout << "Warning: Module "<<sym_map[sym3].mod_num<<": "
			<< sym3 << " was defined" << " but never used" << std::endl;
		}
	}
	std::cout << std::endl;
	return 0;
}