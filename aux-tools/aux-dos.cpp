#include "include/pugixml.cpp"
#include "include/yhzhang.h"
#include <iostream>
#include <vector>
#include <numeric>
#include <cstdio>

using namespace std;
using namespace pugi;
using namespace zyh;

// Global
int FLAG = 1;

// for convenient
Range<int> NEDOSRange;
Range<int> OrbitRange;
Range<int> SpinRange;
vector<string> OrbitLabel{ "-s", "-py", "-pz", "-px", "-dxy", "-dyz", "-dz2", "-dxz", "-dx2", "-f1","-f2", "-f3", "-f4", "-f5", "-f6", "-f7" };
vector<string> SpinLabel;
vector<vector<string>> SpinLabelAll{ { "" }, { "<up>", "<dn>" } };
vector<int> SpinSign;
vector<vector<int>> SpinSignAll{ { 1 }, { 1, -1 } };

class DOS
{
public:
	vector<vector<vector<double>>> Energy;

	explicit DOS(const int& ISPIN, const int& NEDOS, const xml_node& node)
	{
		// Init
		double tmp;
		this->Energy =
			vector<vector<vector<double>>>(ISPIN,
				vector<vector<double>>(NEDOS,
					vector<double>(3)));

		auto iter1 = node.begin();
		for (auto s : SpinRange)
		{
			auto iter2 = iter1->begin();
			for (auto d : NEDOSRange)
			{
				split(iter2->child_value(), this->Energy[s][d], tmp);
				++iter2;
			}
			++iter1;
		}
	}
};

class pDOS
{
public:
	vector<vector<vector<vector<double>>>> pdos;

	pDOS(const int& NIONS, const int& ISPIN, const int& NEDOS, const int& NORBIT, const xml_node& node)
	{
		// Init
		double tmp;
		this->pdos =
			vector<vector<vector<vector<double>>>>(NIONS,
				vector<vector<vector<double>>>(ISPIN,
					vector<vector<double>>(NEDOS,
						vector<double>(NORBIT))));
		vector<double> save(NORBIT + 1);

		auto iter1 = node.begin();
		for (auto i = 0; i < NIONS; ++i)
		{
			auto iter2 = iter1->begin();
			for (auto s : SpinRange)
			{
				auto iter3 = iter2->begin();
				for (auto d : NEDOSRange)
				{
					split(iter3->child_value(), save, tmp);
					this->pdos[i][s][d].assign(save.begin() + 1, save.end());
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
	if (argc > 1) { stoval(argv[1], FLAG); }

	// Help
	cout << "\t[1]: pdos(Atom Type),           [2]: pdos(Atom)" << endl;
	cout << "\t[3]: pdos(Atom Type & Orbital), [4]: pdos(Atom & Orbital)" << endl;
	cout << "\tSelected flag = " << FLAG << endl;
	cout << "\tdos restore ... ";

	int index;
	xml_document vasp;
	string filename = "vasprun.xml";
	const auto result = vasp.load_file(filename.c_str());
	if (!result) { cout << "Read '" << filename << "' failed ..." << endl; exit(1); }

#define XNODE(A)   vasp.select_node(A).node()
#define XVALU(A)   vasp.select_node(A).node().child_value()
#define XNODES(A)  vasp.select_nodes(A)
	const auto NEDOSPath = "/modeling/parameters[1]/separator[@name='dos']/i[@name='NEDOS']";
	const auto FERMIPath = "/modeling/calculation/dos/i[@name='efermi']";
	const auto AtomNPath = "/modeling/atominfo/array[@name='atomtypes']/set/rc/c[1]";
	const auto AtomTPath = "/modeling/atominfo/array[@name='atomtypes']/set/rc/c[2]";
	const auto NORBITPath = "/modeling/calculation/dos/partial/array/field";
	const auto ISPINPath = "/modeling/parameters[1]/separator[@name='electronic']/separator[@name='electronic spin']/i[@name='ISPIN']";
	const auto totalPath = "/modeling/calculation/dos/total/array/set";
	const auto partialPath = "/modeling/calculation/dos/partial/array/set";

	// Get data from vasprun.xml
	const auto NEDOS = stoi(XVALU(NEDOSPath));
	NEDOSRange.xrange(NEDOS);

	const auto FERMI = stod(XVALU(FERMIPath));

	auto iter = XNODES(AtomNPath);
	vector<int> AtomN;
	for (auto it : iter) { AtomN.push_back(stoi(it.node().child_value())); }
	const auto NIONS = accumulate(AtomN.begin(), AtomN.end(), 0);

	iter = XNODES(AtomTPath);
	vector<string> AtomT;
	for (auto it : iter)
	{
		string tmp = it.node().child_value();
		tmp.erase(tmp.find_last_not_of(' ') + 1);
		AtomT.push_back(tmp);
	}

	iter = XNODES(NORBITPath);
	const auto NORBIT = iter.size() - 1;
	OrbitRange.xrange(NORBIT);

	const auto ISPIN = stoi(XVALU(ISPINPath));
	SpinRange.xrange(ISPIN);
	SpinLabel = SpinLabelAll[ISPIN - 1];
	SpinSign = SpinSignAll[ISPIN - 1];

	// Total Energy
	auto node = XNODE(totalPath);
	DOS total(ISPIN, NEDOS, node);

	// Partial Energy and contribution
	node = XNODE(partialPath);
	pDOS partial(NIONS, ISPIN, NEDOS, NORBIT, node);

	const auto num_start = AtomN.begin(), num_end = AtomN.end();
	auto num = num_start;
	const auto type_start = AtomT.begin(), type_end = AtomT.end();
	auto type = type_start;

	// Open file
	const auto data = fopen("dos.dat", "w");
	// Title
	fprintf(data, "%9s", "Energy");
	for (auto& s : SpinRange) { fprintf(data, "%10s", ("DOS" + SpinLabel[s]).c_str()); }
	for (auto& s : SpinRange) { fprintf(data, "%12s", ("iDOS" + SpinLabel[s]).c_str()); }
	switch (FLAG)
	{
	case 1:
		for (auto& t : AtomT) for (auto& s : SpinRange) { fprintf(data, "%12s", (t + SpinLabel[s]).c_str()); }
		break;

	case 2:
		for (type = type_start, num = num_start; type != type_end && num != num_end; ++type, ++num)
			for (auto n = 0; n < *num; ++n) for (auto& s : SpinRange) { fprintf(data, "%12s", (*type + to_string(n + 1) + SpinLabel[s]).c_str()); }
		break;

	case 3:
		for (auto& t : AtomT) for (auto& o : OrbitRange) for (auto& s : SpinRange) { fprintf(data, "%12s", (t + OrbitLabel[o] + SpinLabel[s]).c_str()); }
		break;

	case 4:
		for (type = type_start, num = num_start; type != type_end && num != num_end; ++type, ++num)
			for (auto n = 0; n < *num; ++n)
				for (auto& o : OrbitRange)
					for (auto& s : SpinRange) { fprintf(data, "%14s", (*type + to_string(n + 1) + OrbitLabel[o] + SpinLabel[s]).c_str()); }
		break;

	default: break;
	}
	fprintf(data, "\n");
	// Content
	for (auto d : NEDOSRange)
	{
		fprintf(data, "%9.4f", total.Energy[0][d][0] - FERMI);
		for (auto& s : SpinRange) { fprintf(data, "%10.4f", total.Energy[s][d][1] * SpinSign[s]); }
		for (auto& s : SpinRange) { fprintf(data, "%12.4f", total.Energy[s][d][2] * SpinSign[s]); }
		switch (FLAG)
		{
		case 1:
			for (type = type_start, num = num_start; type != type_end && num != num_end; ++type, ++num)
				for (auto s : SpinRange)
				{
					auto SUM = 0.0;
					index = accumulate(num_start, num, 0);
					for (auto n = 0; n < *num; ++n, ++index)
					{
						const auto tmp = &partial.pdos[index][s][d];
						for (const auto& i : *tmp) { SUM += i; }
					}
					fprintf(data, "%12.4f", SUM * SpinSign[s]);
				}break;

		case 2:
			for (type = type_start, num = num_start, index = 0; type != type_end && num != num_end; ++type, ++num)
				for (auto n = 0; n < *num; ++n, ++index)
					for (auto s : SpinRange)
					{
						auto SUM = 0.0;
						const auto tmp = &partial.pdos[index][s][d];
						for (const auto& i : *tmp) { SUM += i; }
						fprintf(data, "%12.4f", SUM * SpinSign[s]);
					}break;

		case 3:
			for (type = type_start, num = num_start; type != type_end && num != num_end; ++type, ++num)
				for (auto o : OrbitRange)
					for (auto s : SpinRange)
					{
						auto SUM = 0.0;
						index = accumulate(num_start, num, 0);
						for (auto n = 0; n < *num; ++n, ++index) { SUM += partial.pdos[index][s][d][o]; }
						fprintf(data, "%12.4f", SUM * SpinSign[s]);
					}break;

		case 4:
			for (type = type_start, num = num_start, index = 0; type != type_end && num != num_end; ++type, ++num)
				for (auto n = 0; n < *num; ++n, ++index)
					for (auto o : OrbitRange)
						for (auto s : SpinRange)
						{
							auto SUM = 0.0; SUM += partial.pdos[index][s][d][o]; fprintf(data, "%14.4f", SUM * SpinSign[s]);
						}break;

		default: break;
		} fprintf(data, "\n");
	}
	fclose(data);

	cout << "Done!" << endl;
	return 0;
}
