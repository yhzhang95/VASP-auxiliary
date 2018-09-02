#define _USE_MATH_DEFINES
#include <pugixml.cpp>
#include <yhzhang.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <cstdio>
#include <cmath>
#include <valarray>

using namespace std;
using namespace pugi;
using namespace zyh;

// Global
int FLAG = 1;
bool SPLIT_FLAG = false;
string OUTCAR = "OUTCAR";


// for convenient
Range<int> OrbitRange;
Range<int> BandRange;
Range<int> KpointRange;
Range<int> SpinRange;
vector<string> SpinLabel;
vector<string> OrbitLabel{ "-s", "-py", "-pz", "-px", "-dxy", "-dyz", "-dz2", "-dxz", "-dx2", "-f1","-f2", "-f3", "-f4", "-f5", "-f6", "-f7" };
vector<vector<string>> SpinLabelAll{ { "" },{ "<up>", "<dn>" } };


class OPT
{
public:
	int             _size;
	vector<int>     single_char;
	vector<string>  single_opt;
	vector<string>  argopt;

	OPT(const int argc, char** argv, const string& sopt, const string& lopt)
	{
		const auto ssize = sopt.size(), lsize = lopt.size();
		for (auto index = 1; index < argc; ++index)
		{
			auto check = false;
			for (auto i = 0; i < ssize; ++i)
			{
				if (argv[index] == string("-") + sopt[i])
				{
					this->single_char.push_back(sopt.c_str()[i]);
					this->single_opt.emplace_back("NULL");
					check = true;
				}
			}
			if (check) continue;

			for (auto i = 0; i < lsize; i++)
			{
				if (argv[index] == string("-") + lopt[i])
				{
					this->single_char.push_back(lopt.c_str()[i]);
					this->single_opt.emplace_back(argv[++index]);
					check = true;
				}
			}
			if (check) continue;

			this->argopt.emplace_back(argv[index]);
		}
		this->_size = this->single_char.size();
	}
};



class KPOINT
{
public:
	vector<string> label;
	vector<double> coord;
	int            _size;

	KPOINT(const xml_node& node, const vector<vector<double>>& B)
	{
		// buff and tmp
		char buff[20];
		auto index = 0;
		double tmp;

		// Init
		this->_size = distance(node.begin(), node.end());
		this->coord.resize(this->_size);
		this->label.resize(this->_size);
		vector<vector<double>> coordxyz(this->_size, vector<double>(3));
		for (auto it = node.begin(); it != node.end(); ++it, ++index)
		{
			split(it->child_value(), coordxyz[index], tmp);
			snprintf(buff, sizeof(buff), "(%4.2f|%4.2f|%4.2f)", coordxyz[index][0], coordxyz[index][1], coordxyz[index][2]);
			this->label[index] = buff;
		}


		valarray<double> subbuff(0.0, _size);
		for (index = 1; index < _size; ++index)
		{
			const auto dx = coordxyz[index][0] - coordxyz[index - 1][0];
			const auto dy = coordxyz[index][1] - coordxyz[index - 1][1];
			const auto dz = coordxyz[index][2] - coordxyz[index - 1][2];

			const auto x = dx * B[0][0] + dy * B[1][0] + dz * B[2][0];
			const auto y = dx * B[0][1] + dy * B[1][1] + dz * B[2][1];
			const auto z = dx * B[0][2] + dy * B[1][2] + dz * B[2][2];

			subbuff[index] = sqrt(x * x + y * y + z * z);
			this->coord[index] = subbuff.sum() * 2 * M_PI;
		}
	}
};


class EIGEN
{
public:
	vector<vector<vector<vector<double>>>> Energy;


	explicit EIGEN(const int& ISPIN, const int& NKPOINT, const int& NBAND, const xml_node& node)
	{
		// Init
		this->Energy =
			vector<vector<vector<vector<double>>>>(ISPIN,
				vector<vector<vector<double>>>(NKPOINT,
					vector<vector<double>>(NBAND,
						vector<double>(2))));

		double tmp;
		auto iter1 = node.begin();
		for (auto s : SpinRange)
		{
			auto iter2 = iter1->begin();
			for (auto k : KpointRange)
			{
				auto iter3 = iter2->begin();
				for (auto b : BandRange)
				{
					split(iter3->child_value(), this->Energy[s][k][b], tmp);
					++iter3;
				}
				++iter2;
			}
			++iter1;
		}
	}
};


class PRO
{
public:
	vector<vector<vector<vector<vector<double>>>>> pro;

	PRO(const int& ISPIN, const int& NKPOINT, const int& NBAND, const int& NIONS, const int& NORBIT, const xml_node& node)
	{
		// Init
		this->pro =
			vector<vector<vector<vector<vector<double>>>>>(ISPIN,
				vector<vector<vector<vector<double>>>>(NKPOINT,
					vector<vector<vector<double>>>(NBAND,
						vector<vector<double>>(NIONS,
							vector<double>(NORBIT)))));

		double tmp;
		auto iter1 = node.begin();
		for (auto s : SpinRange)
		{
			auto iter2 = iter1->begin();
			for (auto k : KpointRange)
			{
				auto iter3 = iter2->begin();
				for (auto b : BandRange)
				{
					auto iter4 = iter3->begin();
					for (auto i = 0; i < NIONS; ++i)
					{
						split(iter4->child_value(), this->pro[s][k][b][i], tmp);
						++iter4;
					}
					++iter3;
				}
				++iter2;
			}
			++iter1;
		}
	}
};


int main(const int argc, char** argv)
{
	double tmpd;
	int index;

	// Get option
	OPT optopt(argc, argv, "s", "f");
	for (auto i = 0; i < optopt._size; ++i)
		switch (optopt.single_char[i])
		{
		case 's':SPLIT_FLAG = true; break;
		case 'f':OUTCAR = optopt.single_opt[i]; break;
		default:break;
		}
	for (const auto& flag : optopt.argopt) { FLAG = stoi(flag); }

	// Help
	if (!SPLIT_FLAG)
	{
		cout << "\t=== Usage: aux-band [-s] [-f filename] flag ===" << endl;
		cout << "\t[1]: band(Atom Type),           [2]: band(Atom)" << endl;
		cout << "\t[3]: band(Atom Type & Orbital), [4]: band(Atom & Orbital)" << endl;
		cout << "\tGet E_fermi from '" << OUTCAR << "' & Selected flag = " << FLAG << endl;
		cout << "\tband restore ... ";
	}
	else { cout << "\tSelected Mode = Split\n\tband restore ... "; }

	// Parse xml file
	xml_document vasp;
	string filename = "vasprun.xml";
	const auto result = vasp.load_file(filename.c_str());
	if (!result) { cout << "Read '" << filename << "' failed ..." << endl; exit(1); }

	// Get Fermi
	auto FERMI = 0.0;
	ifstream file(OUTCAR);
	string line;
	while (getline(file, line)) if (line.find("E-fermi") != -1) { auto tmp = split(line); stoval(tmp[2], FERMI); break; }

#define XNODE(A)   vasp.select_node(A).node()
#define XVALU(A)   vasp.select_node(A).node().child_value()
#define XNODES(A)  vasp.select_nodes(A)
	const auto BPath = "/modeling/calculation/structure/crystal/varray[@name='rec_basis']";
	const auto AtomNPath = "/modeling/atominfo/array[@name='atomtypes']/set/rc/c[1]";
	const auto AtomTPath = "/modeling/atominfo/array[@name='atomtypes']/set/rc/c[2]";
	const auto ISPINPath = "/modeling/parameters[1]/separator[@name='electronic']/separator[@name='electronic spin']/i[@name='ISPIN']";
	const auto NBANDPath = "/modeling/parameters[1]/separator[@name='electronic']/i[@name='NBANDS']";
	const auto KPOINTSPath = "/modeling/kpoints/varray[@name='kpointlist']";
	const auto NORBITPath = "/modeling/calculation/projected/array/field";
	const auto NIONSPath = "/modeling/atominfo/atoms";
	const auto eigenvaluesPath = "/modeling/calculation/projected/eigenvalues/array/set";
	const auto projectedPath = "/modeling/calculation/projected/array/set";

	// Get data from vasprun.xml
	auto iter = XNODE(BPath).begin();
	vector<vector<double>> B(3, vector<double>(3));
	for (index = 0; index < 3; ++index, ++iter) { split(iter->child_value(), B[index], tmpd); }

	auto lazy = XNODES(AtomNPath);
	vector<int> AtomN;
	for (auto it : lazy) { AtomN.push_back(stoi(it.node().child_value())); }

	lazy = XNODES(AtomTPath);
	vector<string> AtomT;
	for (auto it : lazy)
	{
		string tmp = it.node().child_value();
		tmp.erase(tmp.find_last_not_of(' ') + 1);
		AtomT.push_back(tmp);
	}

	const auto ISPIN = stoi(XVALU(ISPINPath));
	SpinRange.xrange(ISPIN);
	SpinLabel = SpinLabelAll[ISPIN - 1];

	const auto NBAND = stoi(XVALU(NBANDPath));
	BandRange.xrange(NBAND);
	
	auto node = XNODE(KPOINTSPath);
	const KPOINT KPOINTS(node, B);
	KpointRange.xrange(KPOINTS._size);

	lazy = XNODES(NORBITPath);
	const auto NORBIT = lazy.size();
	OrbitRange.xrange(NORBIT);

	const auto NIONS = stoi(XVALU(NIONSPath));

	// Eigen values
	node = XNODE(eigenvaluesPath);
	EIGEN eigenvalues(ISPIN, KPOINTS._size, NBAND, node);

	// Split band 
	if (SPLIT_FLAG)
	{
		// Open file
		const auto data = fopen("bandsplit.dat", "w");
		char buff[20];
		// Title
		fprintf(data, "%8s", "Coord.");
		fprintf(data, "%20s", "KPOINT     ");
		for (auto s : SpinRange)for (auto b : BandRange) { snprintf(buff, sizeof(buff), "N%d%s", b + 1, SpinLabel[s].c_str()); fprintf(data, "%10s", buff); }
		fprintf(data, "\n");
		for (auto k : KpointRange)
		{
			fprintf(data, "%8.5f", KPOINTS.coord[k]); fprintf(data, "%20s", KPOINTS.label[k].c_str());
			for (auto s : SpinRange) for (auto b : BandRange) { fprintf(data, "%10.4f", eigenvalues.Energy[s][k][b][0] - FERMI); }
			fprintf(data, "\n");
		}
		fclose(data);
		cout << "Done!" << endl;
		return(0);
	}

	// Projected values
	node = XNODE(projectedPath);
	PRO projected(ISPIN, KPOINTS._size, NBAND, NIONS, NORBIT, node);

	// for convenient
	const auto type_start = AtomT.begin(), type_end = AtomT.end();
	auto type = type_start;
	const auto num_start = AtomN.begin(), num_end = AtomN.end();
	auto num = num_start;

	// Open file
	const auto data = fopen("band.dat", "w");
	// Title
	fprintf(data, "%8s", "Coord."); fprintf(data, "%20s", "KPOINT     ");
	switch (FLAG)
	{
	case 1:
		for (auto s : SpinRange)
		{
			fprintf(data, "%12s", ("Energy" + SpinLabel[s]).c_str());
			fprintf(data, "%8s", ("Occ" + SpinLabel[s]).c_str());
			for (const auto& t : AtomT) { fprintf(data, "%8s", (t + SpinLabel[s]).c_str()); }
		} break;

	case 2:
		for (auto s : SpinRange)
		{
			fprintf(data, "%12s", ("Energy" + SpinLabel[s]).c_str());
			fprintf(data, "%8s", ("Occ" + SpinLabel[s]).c_str());
			for (type = type_start, num = num_start; type != type_end && num != num_end; ++type, ++num)
				for (auto n = 0; n < *num; ++n) { fprintf(data, "%10s", (*type + to_string(n + 1) + SpinLabel[s]).c_str()); }
		} break;

	case 3:
		for (auto s : SpinRange)
		{
			fprintf(data, "%12s", ("Energy" + SpinLabel[s]).c_str());
			fprintf(data, "%8s", ("Occ" + SpinLabel[s]).c_str());
			for (auto& t : AtomT) for (auto& o : OrbitRange) { fprintf(data, "%11s", (t + OrbitLabel[o] + SpinLabel[s]).c_str()); }
		} break;

	case 4:
		for (auto s : SpinRange)
		{
			fprintf(data, "%12s", ("Energy" + SpinLabel[s]).c_str());
			fprintf(data, "%8s", ("Occ" + SpinLabel[s]).c_str());
			for (type = type_start, num = num_start; type != type_end && num != num_end; ++type, ++num)
				for (auto n = 0; n < *num; ++n)
					for (auto& o : OrbitRange) { fprintf(data, "%12s", (*type + to_string(n + 1) + OrbitLabel[o] + SpinLabel[s]).c_str()); }
		} break;

	default: break;
	} fprintf(data, "\n");
	// Content
	for (auto b : BandRange)
	{
		for (auto k : KpointRange)
		{
			fprintf(data, "%8.5f", KPOINTS.coord[k]);
			fprintf(data, "%20s", KPOINTS.label[k].c_str());
			switch (FLAG)
			{
			case 1:
				for (auto s : SpinRange)
				{
					fprintf(data, "%12.4f", eigenvalues.Energy[s][k][b][0] - FERMI);
					fprintf(data, "%8.3f", eigenvalues.Energy[s][k][b][1]);
					for (type = type_start, num = num_start, index = 0; type != type_end && num != num_end; ++type, ++num)
					{
						auto SUM = 0.0;
						for (auto n = 0; n < *num; ++n, ++index)
						{
							const auto tmp = &projected.pro[s][k][b][index];
							for (const auto& i : *tmp) { SUM += i; }
						}
						fprintf(data, "%8.4f", SUM);
					}
				} break;

			case 2:
				for (auto s : SpinRange)
				{
					fprintf(data, "%12.4f", eigenvalues.Energy[s][k][b][0] - FERMI);
					fprintf(data, "%8.3f", eigenvalues.Energy[s][k][b][1]);
					for (type = type_start, num = num_start, index = 0; type != type_end && num != num_end; ++type, ++num)
						for (auto n = 0; n < *num; ++n, ++index)
						{
							auto SUM = 0.0;
							const auto tmp = &projected.pro[s][k][b][index];
							for (const auto& i : *tmp) { SUM += i; }
							fprintf(data, "%10.4f", SUM);
						}
				} break;

			case 3:
				for (auto s : SpinRange)
				{
					fprintf(data, "%12.4f", eigenvalues.Energy[s][k][b][0] - FERMI);
					fprintf(data, "%8.3f", eigenvalues.Energy[s][k][b][1]);
					for (type = type_start, num = num_start; type != type_end && num != num_end; ++type, ++num)
					{
						for (auto o : OrbitRange)
						{
							auto SUM = 0.0;
							index = accumulate(num_start, num, 0);
							for (auto n = 0; n < *num; ++n, ++index) { SUM += projected.pro[s][k][b][index][o]; }
							fprintf(data, "%11.4f", SUM);
						}
					}
				} break;

			case 4:
				for (auto s : SpinRange)
				{
					fprintf(data, "%12.4f", eigenvalues.Energy[s][k][b][0] - FERMI);
					fprintf(data, "%8.3f", eigenvalues.Energy[s][k][b][1]);
					for (type = type_start, num = num_start, index = 0; type != type_end && num != num_end; ++type, ++num)
						for (auto n = 0; n < *num; ++n, ++index)
							for (auto o : OrbitRange)
								fprintf(data, "%12.4f", projected.pro[s][k][b][index][o]);
				} break;

			default: break;
			} fprintf(data, "\n");
		} fprintf(data, "\n");
	}
	fclose(data);

	cout << "Done!" << endl;
	return 0;
}
