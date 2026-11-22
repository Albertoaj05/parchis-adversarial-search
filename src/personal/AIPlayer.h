#ifndef __AI_PLAYER_H__
#define __AI_PLAYER_H__
#include "../../include/model/Dice.h"

# include "../../include/model/Attributes.h"
# include "../../include/model/Player.h"
# include "../../include/model/Heuristic.h"


//////////////////////////////// VALORACION TEST ///////////////////////////////////////////
class ValoracionTest: public Heuristic{
   protected:
      virtual float getHeuristic(const Parchis &estado, int jugador) const;
};
////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////// VALORACION 1 //////////////////////////////////////////////
class Valoracion1: public Heuristic{
protected:
   virtual float getHeuristic(const Parchis &estado, int jugador) const;
};
////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////// VALORACION 2 //////////////////////////////////////////////
class Valoracion2: public Heuristic{
protected:
   virtual float getHeuristic(const Parchis &estado, int jugador) const;
};
////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////// VALORACION 3  /////////////////////////////////////////////
class Valoracion3: public Heuristic{
protected:
   virtual float getHeuristic(const Parchis &estado, int jugador) const;
};
////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////// VALORACION DEFINITIVA /////////////////////////////////////
class MejorValoracion: public Heuristic{
protected:
   virtual float getHeuristic(const Parchis &estado, int jugador) const;
};
////////////////////////////////////////////////////////////////////////////////////////////




class AIPlayer : public Player{
protected:
   //Id identificativo del jugador
   const int id;

public:
   /**
    * @brief Constructor de un objeto AIPlayer
    *
    * @param name Nombre del jugador
    */
   inline AIPlayer(const string& name): Player(name), id(1){
   };

   /**
    * @brief Constructor de un objeto AIPlayer
    *
    * @param name Nombre  del jugador
    * @param id Id del jugador
    */
   inline AIPlayer(const string& name, const int id): Player(name), id(id){
   };

   /**
    * @brief Función que percibe el parchís y al jugador actual.
    * Asigna el tablero en actual y el id del jugador.
    *
    * @param p Instancia Parchis que se quiere percibir
    */
   inline virtual void perceive(Parchis& p){ Player::perceive(p); }

   /**
    * @brief Función abstracta que define el movimiento devuelto por el jugador.
    * Llama a la función movePiece con el valor asignado a los parámetros pasados
    * por referencia.
    *
    * @return true
    * @return false
    */
   virtual bool move();

   /**
    * @brief Función que se encarga de decidir el mejor movimiento posible a
    * partir del estado actual del tablero. Asigna a las variables pasadas por
    * referencia el valor de color de ficha, id de ficha y dado del mejor movimiento.
    *
    * @param c_piece Color de la ficha
    * @param id_piece Id de la ficha
    * @param dice Número de dado
    */
   virtual void think(color& c_piece, int& id_piece, int& dice) const;

   /**
   * @brief Método que determina si el player es inteligente (decide el mejor movimiento)
   * o no. True para AIPlayer.
   *
   * @return true
   * @return false
   */
   inline virtual bool canThink() const
   {
      return true;
   }

   // Metodo publico constante que se encarga de realizar un movimiento aleatorio
   void thinkAleatorio(color &c_piece, int &id_piece, int &dice) const;

   // Metodo publico constante que se encarga de mover la ficha más adelantada y por tanto, más cercana a la meta
   void thinkFichaMasAdelantada(color &c_piece, int &id_piece, int &dice) const;

   // Metodo público constante que prioriza jugadas ventajosas como ganar, puntuar o comer, si no puede, usa thinkFichaMasAdelantada
   void thinkMejorOpcion(color &c_piece, int &id_piece, int &dice) const;

   // Metodo público constante que implementa un algoritmo Minimax clásico, pero sin poda
   float Minimax(const Parchis &actual, int jugador, int profundidad, int profundidad_max, color &c_piece, int &id_piece, int &dice, Heuristic *heuristic) const;

   // Metodo público constante que implementa el mismo algoritmo Minimax, pero limitado a un número de nodos concreto
   float Minimax_Limitado(const Parchis &actual, int jugador, int profundidad, int profundidad_max, color &c_piece, int &id_piece, int &dice, Heuristic *heuristic) const;

   // Metodo público constante que implementa de nuevo Minimax pero optimizado gracias a la poda Alfa-Beta controlando de la misma forma el límite de nodos
   float Poda_AlfaBeta(const Parchis &actual, int jugador, int profundidad, int profundidad_max, color &c_piece, int &id_piece, int &dice, float alpha, float beta, Heuristic *heuristic) const;

   // Metodo público constante que implementa la Poda Alfa-Beta con profundidad dinámica según el número de nodos hijos
   float Poda_AlfaBeta_Dinamica(const Parchis &actual, int jugador, int profundidad, int profundidad_max, color &c_piece, int &id_piece, int &dice, float alpha, float beta, Heuristic *heuristic) const;

   // Metodo público constante que implementa la Poda Alfa-Beta realizando una ordenación previa de los nodos hijos
   float Poda_AlfaBetaOrdenada(const Parchis &actual, int jugador, int profundidad, int profundidad_max, color &c_piece, int &id_piece, int &dice, float alpha, float beta, Heuristic *heuristic) const;

};

#endif
