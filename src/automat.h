
#ifndef _AUTOMAT_H_
#define _AUTOMAT_H_

#include <string>
#include <set>
#include <map>
#include <list>

using namespace std;

//finite state automat with actions associated to transitions
template <class T> class automat
{
public:
	typedef void (T:: *action)(char c);
//private:
	struct state;
	struct transition
	{
		transition(state *dest, const string &input, action act) :
			input(input.begin(), input.end()),
			dest(dest),
			act(act)
		{}

		set<char> input;
		state *dest;
		action act;
	};
	struct state
	{
		void add_transition(state *dest, const string &input, action act)
		{
			//int transition_count = transitions.size();
			transitions.push_back(transition(dest, input, act));
		}
		transition *get_transition(char c)
		{
			//int transition_count = transitions.size();
			for (typename list<transition>::iterator I = transitions.begin() ; I!=transitions.end() ; I++)
			{
				if ((*I).input.find(c) != (*I).input.end())
					return &(*I);
			}
			return NULL;
		}

		list<transition> transitions;
	};

public:
//create an automat
	automat(T *output) :
		output(output)
	{
		begin = &(states["begin"]);
		end = &(states["end"]);
		error = &(states["error"]);
		eof = &(states["eof"]);
		reset();
	}
	//states are added automatically
	void add_transition(const char *src, const char *dest, const string in, action act = 0)
	{
		states[src].add_transition(&(states[dest]),in,act);
	}

//parse using the automat
	void reset() {current = begin;}
	void process(char c)
	{
		if ((current == end) || (current == error))
			current = error;
		else
		{
			transition *transit = current->get_transition(c);
			if (transit==NULL)
				current = error;
			else
			{
				if (transit->act != 0)
					(output->*(transit->act))(c);
				current = transit->dest;
			}
		}
	}
	bool is_end() {return current == end;}
	bool is_error() {return current == error;}
	bool is_eof() {return current == eof;}
	void read_from(istream &in)
	{
		reset();
		char c;
		while (!is_end() && !is_error() && !in.eof())
		{
			in.read(&c,1);
			process(c);
		}
		if (in.eof())
		{
			if (current == begin)
				current = eof;
			else
				current = error;
		}
	}

private:
	T *output; //replace by &
	map<string,state> states;
	state *begin;
	state *end;
	state *error;
	state *eof;
	state *current;
};

#endif //_AUTOMAT_H_
