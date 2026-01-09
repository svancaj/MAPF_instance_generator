#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>
#include <algorithm>
#include <math.h>
#include <vector>
#include <cmath>
#include <queue>

using namespace std;

//constexpr int AGENTS = 300;

/*
 map size 10x10 -> 50 agents
 map size 20x20 -> 200 agents
 map size >> 	-> 400 agents
*/

int create(int, string, int);
void set_agents(vector<vector<char> >&, vector<pair<int, int> >&, vector<pair<int, int> >&, int, int);
bool contains(vector<pair<int, int> >&, int, int);
pair<int,int> get_goal(vector<vector<char> >& map, int x, int y, int distance);

int main(int argc, char** argv)
{
	string map;
	if (argc > 1)
		map = argv[1];
	else
		return -1;

	vector<int> locations = {1}; // 1 = random, 2 = centered, 3 = same path lengths (+-5%), 4 = 3 + start in similar location
	int number_of_instances = 10;

	srand(time(NULL));
	for (size_t i = 0; i < locations.size(); i++)
		create(number_of_instances, map, locations[i]);
}

int create(int number_of_instances, string map_type, int locations_type)
{
	vector<vector<char> > map;
	string map_name;
	string locations_name;

	size_t found = map_type.find_last_of('/');
	if (found != string::npos)
		map_name = map_type.substr(found + 1);
	else
		map_name = map_type;
	map_name.erase(map_name.end()-4, map_name.end());

	ifstream in;
	in.open(map_type);
	if (!in.is_open())
	{
		cout << "can not open " << map_type << endl;
		return -1;
	}

	string s_dump;
	int height, width;
	getline(in, s_dump); // first line - type

	in >> s_dump >> height;
	in >> s_dump >> width;
	in >> s_dump; // map

	int agents = 400;

	if (height == 20 || height == 22) // warehouse 20x20 is acctualy 18x22
		agents = 200;
	if (height == 10)
		agents = 50;

	// graph
	map = vector<vector<char> >(height, vector<char>(width, '@'));
	int number_of_vertices = 0;

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			in >> map[i][j];

	in.close();

	switch (locations_type)
	{
	case 1:
		locations_name = "random";
		break;
	case 2:
		locations_name = "grouped";
		break;
	case 3:
		locations_name = "distance";
		break;
	case 4:
		locations_name = "condensed";
		break;
	default:
		cout << "Illegal starting and goal type of map!" << endl;
		return -1;
	}
	
	for (int instance_nr = 0; instance_nr < number_of_instances; instance_nr++)
	{
		// name
		stringstream ss;
		//ss << map_name << "-" << locations_name << "-" << instance_nr << ".scen";
		ss << map_name << "_" << instance_nr << ".scen";
		string filename = ss.str();

		// start and goals
		vector<pair<int, int> > start;
		vector<pair<int, int> > goal;

		set_agents(map, start, goal, locations_type, agents);

		// print
		ofstream out1(string("created_instances/").append(filename));
		streambuf *coutbuf1 = cout.rdbuf(); //save old buf
		cout.rdbuf(out1.rdbuf()); //redirect std::cerr to out.txt!

		cout << "version 1" << endl;

		for (size_t i = 0; i < start.size(); i++)
		{
			cout << "0\t" << map_name << ".map\t" << map.size() << "\t" << map.size() << "\t";
			cout << start[i].second << "\t" << start[i].first << "\t" << goal[i].second << "\t" << goal[i].first << "\t";
			cout << "0" << endl;
		}
		
		cout.rdbuf(coutbuf1); //reset to standard output again
	}

	return 0;
}

void set_agents(vector<vector<char> >& map, vector<pair<int, int> >& start, vector<pair<int, int> >& goal, int locations_type, int AGENTS)
{
	if (locations_type == 1) // random
	{
		int placed = 0;
		while (placed < AGENTS)
		{
			int x = rand() % map.size();
			int y = rand() % map.size();
			if (map[x][y] != '.')
				continue;
			if (contains(start, x, y))
				continue;
			start.push_back(make_pair(x, y));
			placed++;
		}

		placed = 0;
		while (placed < AGENTS)
		{
			int x = rand() % map.size();
			int y = rand() % map.size();
			if (map[x][y] != '.')
				continue;
			if (contains(goal, x, y))
				continue;
			goal.push_back(make_pair(x, y));
			placed++;
		}
	}

	if (locations_type == 2) // half is grouped
	{
		int placed = 0;
		int box_size = ceil(sqrt(AGENTS * 0.8));
		cout << box_size << endl;

		int x_center = rand() % (map.size() - box_size);
		int y_center = rand() % (map.size() - box_size);

		while (placed < AGENTS)
		{
			if (placed % 2) // random
			{
				int x = rand() % map.size();
				int y = rand() % map.size();
				if (map[x][y] != '.')
					continue;
				if (contains(start, x, y))
					continue;
				start.push_back(make_pair(x, y));
				placed++;
			}
			else // center
			{
				int x = x_center + rand() % box_size;
				int y = y_center + rand() % box_size;
				if (map[x][y] != '.')
					continue;
				if (contains(start, x, y))
					continue;
				start.push_back(make_pair(x, y));
				placed++;
			}
		}

		placed = 0;
		while (placed < AGENTS)
		{
			if (placed % 2) // random
			{
				int x = rand() % map.size();
				int y = rand() % map.size();
				if (map[x][y] != '.')
					continue;
				if (contains(goal, x, y))
					continue;
				goal.push_back(make_pair(x, y));
				placed++;
			}
			else // center
			{
				int x = x_center + rand() % box_size;
				int y = y_center + rand() % box_size;
				if (map[x][y] != '.')
					continue;
				if (contains(goal, x, y))
					continue;
				goal.push_back(make_pair(x, y));
				placed++;
			}
		}
	}

	if (locations_type == 3) // agents have same paht lenghts
	{
		// manhatan distance between 2 random points
		int distance = rand() % map.size() + rand() % map.size();
		cout << "using distance " << distance << endl;

		int placed = 0;
		while (placed < AGENTS)
		{
			int x = rand() % map.size();
			int y = rand() % map.size();
			if (map[x][y] != '.')
				continue;
			if (contains(start, x, y))
				continue;
			pair<int, int> goal_candidate = get_goal(map, x, y, distance);
			if (contains(goal, goal_candidate.first, goal_candidate.second))
				continue;
			start.push_back(make_pair(x, y));
			goal.push_back(goal_candidate);
			placed++;
		}
	}

	if (locations_type == 4) // agents have same paht lenghts + start at similar location
	{
		int box_size = ceil(sqrt(AGENTS));
		cout << box_size << endl;

		int x_center = rand() % (map.size() - box_size);
		int y_center = rand() % (map.size() - box_size);

		int distance = box_size + rand() % map.size();// + (rand() % map.size())/2;
		cout << "using distance " << distance << endl;

		int placed = 0;
		while (placed < AGENTS)
		{
			int x = x_center + rand() % box_size;
			int y = y_center + rand() % box_size;
			if (map[x][y] != '.')
				continue;
			if (placed % 2) // either start or goal in the box
			{
				if (contains(start, x, y))
					continue;
				pair<int, int> goal_candidate = get_goal(map, x, y, distance);
				if (contains(goal, goal_candidate.first, goal_candidate.second))
					continue;
				start.push_back(make_pair(x, y));
				goal.push_back(goal_candidate);
			}
			else
			{
				if (contains(goal, x, y))
					continue;
				pair<int, int> start_candidate = get_goal(map, x, y, distance);
				if (contains(start, start_candidate.first, start_candidate.second))
					continue;
				goal.push_back(make_pair(x, y));
				start.push_back(start_candidate);
			}
			placed++;
		}
	}
}


bool contains(vector<pair<int, int> >& vc, int x, int y)
{
	if (x == -1)
		return true; // invalid start/goal -- do not add

	for (size_t i = 0; i < vc.size(); i++)
		if (vc[i].first == x && vc[i].second == y)
			return true;
	return false;
}


pair<int,int> get_goal(vector<vector<char> >& map, int x, int y, int distance)
{
	vector<vector<int> > BFS_map = vector<vector<int> >(map.size(), vector<int>(map[0].size(), -1));
	vector<pair<int,int> > candidates;

	BFS_map[x][y] = 0;

	queue<pair<int,int> > que;
	que.push(make_pair(x,y));

	while(!que.empty())
	{
		pair<int,int> v = que.front();
		que.pop();

		if (BFS_map[v.first][v.second] >= distance + distance * 0.05)
			continue;

		if (BFS_map[v.first][v.second] >= distance - distance * 0.05)
			candidates.push_back(v);

		if (v.first > 0 && map[v.first - 1][v.second] != '@' && BFS_map[v.first - 1][v.second] == -1)
		{
			BFS_map[v.first - 1][v.second] = BFS_map[v.first][v.second] + 1;
			que.push(make_pair(v.first - 1, v.second));
		}
		if (v.first < map.size() - 1 && map[v.first + 1][v.second] != '@' && BFS_map[v.first + 1][v.second] == -1)
		{
			BFS_map[v.first + 1][v.second] = BFS_map[v.first][v.second] + 1;
			que.push(make_pair(v.first + 1, v.second));
		}
		if (v.second > 0 && map[v.first][v.second - 1] != '@' && BFS_map[v.first][v.second - 1] == -1)
		{
			BFS_map[v.first][v.second - 1] = BFS_map[v.first][v.second] + 1;
			que.push(make_pair(v.first, v.second - 1));
		}
		if (v.second < map.size() - 1 && map[v.first][v.second + 1] != '@' && BFS_map[v.first][v.second + 1] == -1)
		{
			BFS_map[v.first][v.second + 1] = BFS_map[v.first][v.second] + 1;
			que.push(make_pair(v.first, v.second + 1));
		}
	}

	if (candidates.size() > 0)
		return candidates[rand() % candidates.size()];
	else
		return make_pair(-1,-1);
}