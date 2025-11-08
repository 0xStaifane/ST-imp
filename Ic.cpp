//Author : William & Stéphane
// Description : Implementation of the Invariant checking

#include<iostream>
#include<vector>
#include<string>
#include<map>
#include<set>
#include<stack>

using namespace std;

// Atomic Proposition class
class AtomicProposition {
    private:
        string name;
        bool value;
    public:
        AtomicProposition(string n, bool v) : name(n), value(v) {}
        string getName()const{ 
            return name; 
        }
        bool getValue() const { return value; }
        void setValue(bool v) { value = v; }
};

// State class
class State {
    private:
        string name;
        bool isInitial;
        map<string, bool> atomicPropositions; //Liste des états des propositions atomiques associées à l'état
        //Pas encore sur pour les transitions
        //map<string, State*> transitions; //Liste des transitions associées à l'état
    public:
        State(string n){
            name = n;
            atomicPropositions = map<string, bool>();
        }

        State(string n, bool initial){
            name = n;
            isInitial = initial;
            atomicPropositions = map<string, bool>();
        }

        string getName(){
            return name;
        }
        bool getIsInitial(){
            return isInitial;
        }
        void setAtomicProposition(string apName, bool value){
            atomicPropositions[apName] = value;
        }
        bool getAtomicProposition(string apName){
            return atomicPropositions[apName];
        }


};

class Expression {
    private:
    public:
};

class ST {
    private:
        string name;
        set<State> states;
        set<State> initialState;
    public:
        ST(string n, set<State> s, set<State> i){
            name = n;
            states = set<State>();
            for(auto state : s){
                if (state.getIsInitial()){
                    initialState.insert(state);
                }
            }
        }


};
