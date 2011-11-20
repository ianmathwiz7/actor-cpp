/* 
 * File:   Actor.h
 * Author: Ameziane NAIT ABDELAZIZ
 * Created on 4 novembre 2011, 17:14
 */

#ifndef ACTOR_H
#define	ACTOR_H

#include "Thread.h"
#include "Closures.h"
#include <list>
#include <string.h>
#include <cassert>
#include <string>


using namespace std;

namespace Acting{


    typedef void* Object;
    typedef void* Memory;
    typedef int   Id;
    typedef int   message_tag;

    /**
     */
    enum actor_status{
        STATUS_SUCCESS,
        STATUS_FAILLUR,
    };

    /**
     */
    struct message_t{
        message_tag tag;
        /**
         * données contigues
         */
        Memory      data;
        Id          sender;
        Id          receiver;
    };


    /**
     * 
     * @param arg
     * @return
     */
    template <typename ARG_T> struct routine_t {
        public:
            virtual actor_status run(ARG_T arg)=0;
    } ;


    /**
     * Toute cette classe doit être thread safe
     * Pour la simple raison que des acteurs peuvent créer d'autres acteurs
     * Dans ce cas, les multiples threads doivent se synchroniser sur certaines
     * données static de la class Actor
     */
    class Actor {
        
    private:
        /**
         */
        Id actor_id;
        
        /**
         */
        string actor_name;

        /**
         */
        Memory actor_stack;

        /**
         */
        list<message_t*> actor_message_box;

        /**
         * 
         */
        static Id compteur;

    public:
        /**
         *
         */
        Actor();

        /**
         *
         * @param orig
         */
        Actor(const Actor& orig);
        
        /**
         * code exécuté par l'acteur
         */
        virtual void act()=0;

        
        /**
         * L'identifiant de l'acteur
         * @return
         */
        Id GetId();

        /**
         *
         * @param name
         */
        void SetName(const string name){
            this->actor_name=name;
        }


        /**
         * 
         * @return
         */
        string  GetName(){
            return this->actor_name;
        }

        
        /**
         * La taille de la file des messages
         * @return
         */
        int GetMessageCount(){
            return this->actor_message_box.size();
        }


        

        /**
         * La méthode doit être estampillé
         */
        template <typename T> int send(Actor* dest,T* message,int tag){

            //printf("%s ----> %s [%d octets]\n",this->GetName().c_str(),dest->GetName().c_str(),sizeof(*message));
            //Ici encapsuler le message dans un paquet
            message_t* pqt = new message_t;
            pqt->data = (Memory) message;
            pqt->receiver=dest->GetId();
            pqt->sender  =this->GetId();
            pqt->tag     = tag;//Actor::compteur++;

            dest->actor_message_box.push_front(pqt);
            return pqt->tag;
        }

        /**
         * Attention au thread safe
         * @param tag
         * @return
         */
        template<typename T> int receive(Actor* src,int tag,T* data){
            //std::cout << "Receiving message tag: "<< tag << std::endl;
            for(list<message_t*>::iterator it=actor_message_box.begin();it!=actor_message_box.end();it++){
                
                if(((*it)->tag==tag) && ((*it)->sender==src->GetId())){

                    void* ret  = memcpy((void*)data,(*it)->data,sizeof((* ((T*)(*it)->data))));
                    assert(ret!=NULL);
                    assert(data!=NULL);

                    
                    //Suppression du message de la boite à lettres
                    delete (*it);
                    actor_message_box.remove(*it);
                    
                    return 0;
                    
                }
            }

            //Si on arrive ici c'est qu'il n'y a pas de message de la file
            //Il faut mettre une attente sur condition
            //Il faudrait que les envoies de messages s'executent dans plusieurs threads

            //Pour faire cela il est nécessaire d'avoir un autre pool de thread qui exécutera
            //le code d'envoie de message
            //Un appel à send ne sera pas bloquant
            //Un appel à recev le sera, parce que il y'aura une attente sur file de message

            //Dans le modèle producteur consommateur send sera une routine de production
            //recv sera une routine de consommation
            assert(0);
            return 0;
        }
        /**
         * Executer une portion de code
         * @param fun : code à exécuter
         */
        template <typename IN,typename OUT> void act(Functional::Function<IN,OUT> &fun){

            //Creer un item
            //L'enfiler
            //Threading::add_job(fun.)
            
        }

        /**
         * Executer une boucle de code
         * @param fun: corps de la boucle
         */
        template <typename IN,typename OUT> void loop(Functional::Function<IN,OUT> &fun){
            
        }

        /**
         * Executer une portion de code tant qu'une condition est vraie
         * @param fun
         */
        template <typename IN,typename OUT> void loopWhile(Functional::Function<IN,OUT> &fun, bool& condition);
        

        virtual ~Actor();
    private:

    };


    Id Actor::compteur;
    /**
     * Constructor
     */
    Actor::Actor(){
        compteur++;
        this->actor_id = compteur;
        this->SetName(string(""));
    }

    /**
     * Destructor
     */
    Actor::~Actor(){

    }


    /**
     * Constructeur par recopie
     */
    Actor::Actor(const Actor& orig){

    }


    Id Actor::GetId(){
        return this->actor_id;
    }
}

#endif	/* ACTOR_H */
