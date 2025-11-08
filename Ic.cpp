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
        set<State*> transitions;
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

//TODo: Class Expression
class Expression {
    private:
    public:
};

class ST {
    private:
        string name;
        set<State> states;
        set<State> initialState;

        set<State> R;           // États visités
        stack<State> U;         // Pile pour l'exploration
        bool b;                 // Indicateur de validité

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
        void addState(State s){
            states.insert(s);
            if (s.getIsInitial()){
                initialState.insert(s);
            }
        }
        
        bool verifyInvariant(Expression expr){
            // TODO: Implémentation de la vérification de l'invariant
            return true; 
        }

        void visit(State s){
           // TODO
        }

        void cheking(Expression expr){
            // Initialisation
            R.clear();
            while(!U.empty()) U.pop();
            b = true;

            // Pousser les états initiaux dans la pile U
            for(auto state : initialState){
                U.push(state);
            }

            // Exploration des états
            while(!U.empty() && b){ // I \ R non vide et b vrai
                visit(U.top());
                U.pop();
            }

            if(b){
                cout << "L'invariant est vérifié pour tous les états accessibles." << endl;
            } else {
                cout << "L'invariant n'est pas vérifié." << endl;
            }   
        }




};
