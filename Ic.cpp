//Author : William & Stéphane
// Description : Implementation of the Invariant checking

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stack>
#include <memory>

using namespace std;

class State;
class Expression;

// Raccourcis pour les shared_ptr
using StatePtr = shared_ptr<State>;
using ExprPtr = shared_ptr<Expression>;

// ============================================================
// Expression logique (AST : Arbre de syntaxe abstraite)
// ============================================================
class Expression {
public:
    // Destructeur, permet la suppression correcte des classes dérivées
    virtual ~Expression() = default;
    virtual bool evaluate(const StatePtr& s) const = 0;
    virtual string toString() const = 0;
};

// Proposition atomique
class AtomicExpr : public Expression {
private:
    string propName;
    
public:
    AtomicExpr(string name) : propName(name) {}
    
    bool evaluate(const StatePtr& s) const override;
    
    string toString() const override {
        return propName;
    }
};

// Négation
class NotExpr : public Expression {
private:
    ExprPtr negExp;
    
public:
    NotExpr(ExprPtr e) : negExp(e) {}
    
    bool evaluate(const StatePtr& s) const override {
        return !negExp->evaluate(s);
    }
    
    string toString() const override {
        return "¬(" + negExp->toString() + ")";
    }
};

// ET logique
class AndExpr : public Expression {
private:
    ExprPtr left;
    ExprPtr right;
    
public:
    AndExpr(ExprPtr l, ExprPtr r){
        left = l;
        right = r;
    }
    
    bool evaluate(const StatePtr& s) const override {
        return left->evaluate(s) && right->evaluate(s);
    }
    
    string toString() const override {
        return "(" + left->toString() + " ∧ " + right->toString() + ")";
    }
};

// OU logique
class OrExpr : public Expression {
private:
    ExprPtr left;
    ExprPtr right;
    
public:
    OrExpr(ExprPtr l, ExprPtr r) : left(l), right(r) {}
    
    bool evaluate(const StatePtr& s) const override {
        return left->evaluate(s) || right->evaluate(s);
    }
    
    string toString() const override {
        return "(" + left->toString() + " ∨ " + right->toString() + ")";
    }
};

// Implication
class ImpliesExpr : public Expression {
private:
    ExprPtr left;
    ExprPtr right;
    
public:
    ImpliesExpr(ExprPtr l, ExprPtr r) : left(l), right(r) {}
    
    bool evaluate(const StatePtr& s) const override {
        // A → B <==> ¬A ∨ B
        return !left->evaluate(s) || right->evaluate(s);
    }
    
    string toString() const override {
        return "(" + left->toString() + " → " + right->toString() + ")";
    }
};

// ============================================================
// State class
// ============================================================
class State {
private:
    string name;
    bool isInitial;
    set<string> atomicPropositions; // Contient uniquement les propositions VRAIES    

    public:
    State(string n, bool initial = false) 
        : name(n), isInitial(initial) {}
    
    string getName() const {
        return name;
    }
    
    bool getIsInitial() const {
        return isInitial;
    }
    
    int getAPSize(){
        return atomicPropositions.size();
    }
    
    void setIsInitial(bool initial) {
        isInitial = initial;
    }
    
    void setAtomicProposition(string apName) {
        atomicPropositions.insert(apName);
    }
    
    bool getAtomicProposition(string apName) const {
        // Retourne faux si la proposition atomique est vraie dans cet état
        // Si find ne truve pas l'élément, il retourne atomicPropositions.end() qui est hors des limites du set
        return atomicPropositions.find(apName) != atomicPropositions.end();
    }

    
};

// Implémentation de AtomicExpr::evaluate 
bool AtomicExpr::evaluate(const StatePtr& s) const {
    return s->getAtomicProposition(propName);
}

// ============================================================
// Helper pour comparer les StatePtr dans les sets
// ============================================================
struct StatePtrHash {
    size_t operator()(const StatePtr& s) const {
        return hash<string>()(s->getName());
    }
};

struct StatePtrEqual {
    bool operator()(const StatePtr& a, const StatePtr& b) const {
        return a->getName() == b->getName();
    }
};

// ============================================================
// Système de Transitions
// ============================================================
class ST {
private:
    string name;
    set<StatePtr> states;
    set<StatePtr> initialStates;
    map<StatePtr, set<StatePtr>> transitions;
    
    // Variables pour l'algorithme
    set<StatePtr> R;           // États visités
    stack<StatePtr> U;         // Pile pour l'exploration
    bool b;                    // Indicateur de validité
    
public:
    ST(string n){
        name = n;
        b = true;
    }
    
    // Créer et ajouter un état
    StatePtr createState(string stateName, bool initial = false) {
        StatePtr s = make_shared<State>(stateName, initial);
        states.insert(s);
        if (initial) {
            initialStates.insert(s);
        }
        return s;
    }
    
    // Ajouter un état existant
    void addState(StatePtr s) {
        states.insert(s);
        if (s->getIsInitial()) {
            initialStates.insert(s);
        }
    }
    
    // Définir un état comme initial
    void setInitialState(StatePtr s) {
        s->setIsInitial(true);
        initialStates.insert(s);
        states.insert(s);
    }
    
    // Ajouter une transition
    void addTransition(StatePtr from, StatePtr to) {
        transitions[from].insert(to);
    }
    
    // Obtenir les successeurs d'un état
    set<StatePtr> post(StatePtr s) const {
        auto it = transitions.find(s);
        if (it != transitions.end()) {
            return it->second;
        }
        return set<StatePtr>();
    }
    
    // Vérifie si tous les successeurs ont été visité
    bool allSuccessorsVisited(StatePtr s) const {
        set<StatePtr> successors = post(s);
        for (const auto& succ : successors) {
            if (R.find(succ) == R.end()) {
                return false;
            }
        }
        return true;
    }
    
    // Visite d'un état
    void visit(StatePtr s, ExprPtr expr) {
        U.push(s);
        R.insert(s);
        
        while (!U.empty() && b) {
            StatePtr current = U.top();
            
            if (allSuccessorsVisited(current)) {
                // Tous les successeurs ont été visités
                U.pop();
                
                // Vérifier si l'état satisfait Φ
                b = b && expr->evaluate(current);
                
                if (!b) {
                    cout << "Contre-exemple trouvé à l'état: " << current->getName() << endl;
                    cout << "Chemin vers le contre-exemple dans la pile U" << endl;
                }
            } else {
                // Il reste des successeurs non visités
                set<StatePtr> successors = post(current);
                
                // Choisir un successeur non visité
                for (const auto& succ : successors) {
                    if (R.find(succ) == R.end()) {
                        U.push(succ);
                        R.insert(succ);
                        break; // On en prend un seul
                    }
                }
            }
        }
    }
    
    // Algorithme de vérification principal
    bool checking(ExprPtr expr) {
        // Initialisation
        R.clear();
        while (!U.empty()) U.pop();
        b = true;
        
        cout << "=== Début de la vérification ===" << endl;
        cout << "Infonction membrevariant à vérifier: " << expr->toString() << endl;
        cout << "Nombre d'états initiaux: " << initialStates.size() << endl;
        
        // Explorer depuis chaque état initial
        for (const auto& state : initialStates) {
            if (R.find(state) == R.end() && b) {
                cout << "Exploration depuis l'état initial: " << state->getName() << endl;
                visit(state, expr);
            }
        }
        
        // Résultat
        if (b) {
            cout << "\n L'invariant est vérifié pour tous les états accessibles." << endl;
        } else {
            cout << "\n L'invariant n'est PAS vérifié." << endl;
            
            // Afficher le chemin du contre-exemple
            cout << "\nChemin vers le contre-exemple:" << endl;
            stack<StatePtr> temp;
            while (!U.empty()) {
                temp.push(U.top());
                U.pop();
            }
            while (!temp.empty()) {
                cout << " -> " << temp.top()->getName();
                temp.pop();
            }
            cout << endl;
        }
        
        return b;
    }
    
    // Méthodes utilitaires pour affichage
    /*void printSystem() const {
        cout << "\n=== Système de transitions: " << name << " ===" << endl;
        cout << "États (" << states.size() << "):" << endl;
        for (const auto& s : states) {
            cout << "  - " << s->getName();
            if (s->getIsInitial()) cout << " (initial)";
            cout << " | Propositions: ";
            for (int i = 0; i < s->getAPSize(); i++) {
            // A refaire :DD
                cout << s->getAtomicProposition << i << " ";
            }
            cout << endl;
        }
        
        cout << "\nTransitions:" << endl;
        for (const auto& trans : transitions) {
            for (const auto& to : trans.second) {
                cout << "  " << trans.first->getName() << " -> " << to->getName() << endl;
            }
        }
    }*/
};

// ============================================================
// Builder pour simplifier la création d'expressions
// ============================================================
class ExprBuilder {
public:
    static ExprPtr atom(string name) {
        return make_shared<AtomicExpr>(name);
    }
    
    static ExprPtr notExpr(ExprPtr e) {
        return make_shared<NotExpr>(e);
    }
    
    static ExprPtr andExpr(ExprPtr l, ExprPtr r) {
        return make_shared<AndExpr>(l, r);
    }
    
    static ExprPtr orExpr(ExprPtr l, ExprPtr r) {
        return make_shared<OrExpr>(l, r);
    }
    
    static ExprPtr implies(ExprPtr l, ExprPtr r) {
        return make_shared<ImpliesExpr>(l, r);
    }
};

// ============================================================
// MAIN - Exemple d'utilisation
// ============================================================
int main() {

    // Créer le système de transitions, modélisant deux processus concurents dont l'accès à une section critique est contrôlé par un sémaphore binaire
    ST system("Section_Critique");
    
    // Créer les états
    StatePtr s0 = system.createState("NC1,NC2|Unlock", true);  // État initial
    StatePtr s1 = system.createState("NC1,C2|Lock");
    StatePtr s2 = system.createState("C1,NC2|Lock");
    
    // Définir les propositions atomiques pour chaque état
    s1->setAtomicProposition("C1");
    //s1->setAtomicProposition("q");
    
    s2->setAtomicProposition("C2");
    //s2->setAtomicProposition("q");
    
    // Définir les transitions
    system.addTransition(s0, s1);//Aquire Lock par P1
    system.addTransition(s0, s2);//Aquire Lock par P2
    system.addTransition(s2, s0);//Release Lock par P2
    system.addTransition(s1, s0);//Release Lock par P1
    
    // Afficher le système
    //system.printSystem();
    
    // Créer l'invariant à vérifier: ¬(C1 ∧ C2) (aucun processus dans la section critique)
    cout << "\n\nTest 1: Vérification de ¬(C1 ∧ C2)" << endl;
    ExprPtr invariant = ExprBuilder::notExpr(
        ExprBuilder::andExpr(
            ExprBuilder::atom("C1"),
            ExprBuilder::atom("C2")
        )
    );
    
    // Vérifier l'invariant
    cout << "\n" << string(50, '=') << endl;
    bool result = system.checking(invariant);
    
    cout << "\n" << string(50, '=') << endl;
    
    // Créer le système de transitions pour le feu tricolore
    ST sysFeu("Feu_Tricolore");

    // Créer les états selon le diagramme
    StatePtr q0 = sysFeu.createState("q0", true);  // État initial
    StatePtr q1 = sysFeu.createState("q1");
    StatePtr q2 = sysFeu.createState("q2");

    // Définir les propositions atomiques pour chaque état
    // État q0 : ¬rouge ∧ ¬orange (ni rouge ni orange)

    // État q1 : ¬rouge ∧ orange (orange mais pas rouge)
    q1->setAtomicProposition("orange");

    // État q2 : rouge (rouge, peut-être avec orange - état final)
    q2->setAtomicProposition("rouge");

    // Définir les transitions selon le diagramme
    sysFeu.addTransition(q0, q1);  // ¬orange (de q0 vers q1)
    sysFeu.addTransition(q1, q0);  // ¬rouge ∧ orange (retour de q1 vers q0)
    sysFeu.addTransition(q0, q0);  // orange (boucle sur q0)
    sysFeu.addTransition(q1, q1);  // ¬rouge ∧ ¬orange (boucle sur q1)
    sysFeu.addTransition(q0, q2);  // rouge (de q0 vers q2)

    // Créer l'invariant à vérifier: ¬(rouge ∧ orange)
    cout << "\n\nTest 2: Vérification que le feu ne peut être rouge et orange en même temps" << endl;
    cout << "Vérification de l'invariant: ¬(rouge ∧ orange)" << endl;
    cout << "Cet invariant garantit qu'on ne peut pas avoir rouge ET orange simultanément" << endl;

    ExprPtr inv = ExprBuilder::notExpr(
        ExprBuilder::andExpr(
            ExprBuilder::atom("rouge"),
            ExprBuilder::atom("orange")
        )
    );

    // Vérifier l'invariant
    cout << "\n" << string(50, '=') << endl;
        bool res2 = sysFeu.checking(inv);

    if (res2) {
        cout << "\n L'invariant ¬(rouge ∧ orange) est vérifié !" << endl;
        cout << "Le système garantit qu'on n'aura jamais rouge et orange en même temps." << endl;
    } else {
        cout << "\n Erreur: Il existe un état où rouge ET orange sont vrais simultanément !" << endl;
    }

    // Test supplémentaire : vérifier que "orange" est parfois vrai
    cout << "\n\n" << string(50, '=') << endl;
    cout << "\n\nTest 3: Vérification que 'orange' est accessible" << endl;
    ExprPtr test_orange = ExprBuilder::atom("orange");
    bool res3 = sysFeu.checking(test_orange);

    return 0;
}