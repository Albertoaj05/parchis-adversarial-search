# include "AIPlayer.h"
# include "../../include/model/Parchis.h"
#include <iostream>
using namespace std;

const float masinf = 9999999999.0, menosinf = -9999999999.0;
const float gana = masinf / 10.f, pierde = menosinf / 10.f;
const int num_pieces = 2;
const int PROFUNDIDAD_MINIMAX = 4; // Umbral maximo de profundidad para el metodo MiniMax
const int PROFUNDIDAD_ALFABETA = 6; // Umbral maximo de profundidad para la poda Alfa_Beta
const int CASILLAS = 75; // Número de casillas totales que tiene un tablero de parchís

bool AIPlayer::move(){
   cout << COUT_ORANGE_BOLD << "Realizo un movimiento automatico" << COUT_NOCOLOR << endl;

   color c_piece;
   int id_piece;
   int dice;
   think(c_piece, id_piece, dice);

   cout << COUT_ORANGE_BOLD << "Movimiento elegido: " << str(c_piece) << " " << id_piece << " " << dice << COUT_NOCOLOR << endl;

   actual->movePiece(c_piece, id_piece, dice);
   return true;
}

// Elige el movimiento según el id del jugador. Cada id combina un algoritmo con una heurística,
// así puedo enfrentar unas versiones contra otras sin recompilar (--p1 AI <id>).
//   0     -> Poda Alfa-Beta + ValoracionTest
//   1     -> Poda Alfa-Beta con profundidad dinámica + MejorValoracion   (versión definitiva)
//   2     -> Poda Alfa-Beta con ordenación + MejorValoracion
//   3, 4  -> agentes sencillos sin búsqueda (mejor opción inmediata / ficha más adelantada)
//   5-8   -> Poda Alfa-Beta con Valoracion1, 2, 3 y MejorValoracion
//   9-12  -> Poda dinámica con ValoracionTest, Valoracion1, 2 y 3
//   13-16 -> Poda con ordenación con ValoracionTest, Valoracion1, 2 y 3
void AIPlayer::think(color& c_piece, int& id_piece, int& dice) const{

   float valor = 0.0;
   float alpha = menosinf;
   float beta = masinf;
   ValoracionTest valoracionTest;
   Valoracion1 v1;
   Valoracion2 v2;
   Valoracion3 v3;
   MejorValoracion mejor;
   int jugador = actual->getCurrentPlayerId();

   switch(id)
   {
   case 0:
      valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, &valoracionTest);
      break;
   case 1:
      valor = Poda_AlfaBeta_Dinamica(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, &mejor);
      break;
   case 2:
      valor = Poda_AlfaBetaOrdenada(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, &mejor);
      break;
   case 3:
      thinkMejorOpcion(c_piece, id_piece, dice);
      break;
   case 4 :
      thinkFichaMasAdelantada(c_piece, id_piece, dice);
      break;
   case 5:
      valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, &v1);
      break;
   case 6:
      valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, &v2);
      break;
   case 7:
      valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, &v3);
      break;
   case 8:
      valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, &mejor);
      break;
   case 9:
      valor = Poda_AlfaBeta_Dinamica(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, &valoracionTest);
      break;
   case 10:
      valor = Poda_AlfaBeta_Dinamica(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, &v1);
      break;
   case 11:
      valor = Poda_AlfaBeta_Dinamica(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, &v2);
      break;
   case 12:
      valor = Poda_AlfaBeta_Dinamica(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, &v3);
      break;
   case 13:
      valor = Poda_AlfaBetaOrdenada(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, &valoracionTest);
      break;
   case 14:
      valor = Poda_AlfaBetaOrdenada(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, &v1);
      break;
   case 15:
      valor = Poda_AlfaBetaOrdenada(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, &v2);
      break;
   case 16:
      valor = Poda_AlfaBetaOrdenada(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, &v3);
      break;
   default :
      thinkMejorOpcion(c_piece, id_piece, dice);
      break;
   }
   cout << "Valor MiniMax: " << valor << " Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;
}

// Movimiento aleatorio: elige un dado al azar y, con ese dado, una ficha al azar.
// Es el punto de partida; thinkFichaMasAdelantada la reutiliza para escoger el dado.
void AIPlayer::thinkAleatorio(color& c_piece, int& id_piece, int& dice) const
{
   int player = actual->getCurrentPlayerId();

   // Dados que puedo usar en este turno y elijo uno al azar
   vector<int> current_dices = actual->getAvailableNormalDices(player);
   dice = current_dices[rand() % current_dices.size()];

   // Fichas que se pueden mover con ese dado
   vector<tuple<color, int>> current_pieces = actual->getAvailablePieces(player, dice);

   if (current_pieces.size() > 0){
      int random_id = rand() % current_pieces.size();
      c_piece = get<0>(current_pieces[random_id]);
      id_piece = get<1>(current_pieces[random_id]);
   }
   else{
      // No puedo mover ninguna ficha con ese dado: paso turno indicando mi color
      id_piece = SKIP_TURN;
      c_piece = actual->getCurrentColor();
   }
}

// Mueve la ficha más adelantada, es decir, la más cercana a la meta
void AIPlayer::thinkFichaMasAdelantada(color &c_piece, int &id_piece, int &dice) const
{
   // El dado lo elijo igual que el jugador aleatorio; lo que cambia es qué ficha muevo
   thinkAleatorio(c_piece, id_piece, dice);

   int player = actual->getCurrentPlayerId();
   vector<tuple<color, int>> current_pieces = actual->getAvailablePieces(player, dice);
   int id_ficha_mas_adelantada = -1;
   color col_ficha_mas_adelantada = none;
   int min_distancia_meta = 9999;
   for (int i = 0; i < current_pieces.size(); i++)
   {
      color col = get<0>(current_pieces[i]);
      int id = get<1>(current_pieces[i]);
      int distancia_meta = actual->distanceToGoal(col, id);
      if (distancia_meta < min_distancia_meta)
      {
         min_distancia_meta = distancia_meta;
         id_ficha_mas_adelantada = id;
         col_ficha_mas_adelantada = col;
      }
   }
   // Si no hay ninguna ficha que mover, paso turno
   if (id_ficha_mas_adelantada == -1)
   {
      id_piece = SKIP_TURN;
      c_piece = actual->getCurrentColor();
   }
   else
   {
      id_piece = id_ficha_mas_adelantada;
      c_piece = col_ficha_mas_adelantada;
   }
}

// Agente voraz: mira solo un movimiento hacia delante. Si alguna jugada come, mete una ficha en meta
// o gana la partida, la elige; si no, mueve la ficha más adelantada.
void AIPlayer::thinkMejorOpcion(color &c_piece, int &id_piece, int &dice) const
{
   // Recorro todos los estados hijo (uno por cada movimiento posible)
   ParchisBros hijos = actual->getChildren();
   bool me_quedo_con_esta_accion = false;

   for (ParchisBros::Iterator it = hijos.begin();
   it != hijos.end() and !me_quedo_con_esta_accion; ++it)
   {
      Parchis siguiente_hijo = *it;
      // Si con este movimiento como, llego a meta o gano, me quedo con él y dejo de buscar
      if (siguiente_hijo.isEatingMove() or
      siguiente_hijo.isGoalMove() or
      (siguiente_hijo.gameOver() and siguiente_hijo.getWinner() == this->jugador))
      {
         me_quedo_con_esta_accion = true;
         c_piece = it.getMovedColor();
         id_piece = it.getMovedPieceId();
         dice = it.getMovedDiceValue();
      }
   }
   // Ninguna jugada destaca: muevo la ficha más adelantada
   if(!me_quedo_con_esta_accion){
      thinkFichaMasAdelantada(c_piece, id_piece, dice);
   }
}

// Minimax clásico, sin poda. Recorre el árbol hasta profundidad_max o hasta que acaba la partida.
// En los nodos MAX (mi turno) me quedo con el máximo y en los MIN (turno del rival) con el mínimo.
// Solo en la raíz (profundidad 0) guardo qué movimiento lleva al mejor valor.
float AIPlayer::Minimax(const Parchis &actual, int jugador, int profundidad, int profundidad_max, color &c_piece, int &id_piece, int &dice, Heuristic *heuristic) const
{
   if (profundidad == profundidad_max || actual.gameOver())
   { // Nodo terminal o profundidad límite: llamo a la función heurística
      return heuristic->evaluate(actual, jugador);
   }
   // Comparo mi jugador con el actual de la partida para saber en qué tipo de nodo estoy
   else if (actual.getCurrentPlayerId() == jugador)
   { // Nodo MAX
      float valor = menosinf; // Inicialización lo más pequeña posible para ir calculando el máximo
      // Obtengo los hijos del nodo actual y los recorro
      ParchisBros rama = actual.getChildren();
      for (ParchisBros::Iterator it = rama.begin(); it != rama.end(); ++it)
      {
         Parchis nuevo_hijo = *it;
         // Búsqueda en profundidad (llamada recursiva)
         float new_val = Minimax(nuevo_hijo, jugador, profundidad + 1, profundidad_max, c_piece, id_piece, dice,
         heuristic);
         if (new_val > valor)
         {
            // Me voy quedando con el máximo
            valor = new_val;
            if (profundidad == 0)
            {
               // Almaceno el movimiento que me ha llevado al mejor valor (solo en la raíz)
               c_piece = it.getMovedColor();
               id_piece = it.getMovedPieceId();
               dice = it.getMovedDiceValue();
            }
         }
      }
      return valor;
   }
   else
   { // Nodo MIN
      float valor = masinf; // Inicialización lo más grande posible para ir calculando el mínimo
      // Obtengo los hijos del nodo actual y los recorro
      ParchisBros rama = actual.getChildren();
      for (ParchisBros::Iterator it = rama.begin(); it != rama.end(); ++it)
      {
         Parchis nuevo_hijo = *it;
         // Búsqueda en profundidad (llamada recursiva)
         float new_val = Minimax(nuevo_hijo, jugador, profundidad + 1, profundidad_max, c_piece, id_piece, dice,
         heuristic);
         // Me voy quedando con el mínimo
         if (new_val < valor)
         {
            valor = new_val;
         }
      }
      return valor;
   }
}

// Mismo Minimax, pero respetando el límite de nodos que impone el motor (NodeCounter).
// Si se alcanza el límite, devuelvo el mejor valor encontrado hasta ese momento.
float AIPlayer::Minimax_Limitado(const Parchis &actual, int jugador, int profundidad, int profundidad_max, color &c_piece, int &id_piece, int &dice, Heuristic *heuristic) const
{
   if (profundidad == profundidad_max || actual.gameOver())
   { // Nodo terminal o profundidad límite: llamo a la función heurística
      return heuristic->evaluate(actual, jugador);
   }
   else if (actual.getCurrentPlayerId() == jugador)
   { // Nodo MAX
      float valor = menosinf;
      // Obtengo los hijos del nodo actual y los recorro
      ParchisBros rama = actual.getChildren();
      for (ParchisBros::Iterator it = rama.begin(); it != rama.end(); ++it)
      {
         Parchis nuevo_hijo = *it;
         // Verificar si hemos alcanzado el límite
         if (NodeCounter::isLimitReached())
         {
            cout << "Límite de nodos alcanzado, devolviendo el mejor nodo parcial" << endl;
            if(profundidad == 0)
            {
               c_piece = it.getMovedColor();
               id_piece = it.getMovedPieceId();
               dice = it.getMovedDiceValue();
            }
            return valor;
         }
         // Búsqueda en profundidad (llamada recursiva)
         float new_val = Minimax_Limitado(nuevo_hijo, jugador, profundidad + 1, profundidad_max, c_piece, id_piece, dice, heuristic);
         if (new_val > valor)
         {
            // Me voy quedando con el máximo
            valor = new_val;
            if (profundidad == 0)
            {
               // Almaceno el movimiento que me ha llevado al mejor valor (solo en la raíz)
               c_piece = it.getMovedColor();
               id_piece = it.getMovedPieceId();
               dice = it.getMovedDiceValue();
            }
         }
      }
      return valor;
   }
   else
   {
      // Nodo MIN
      float valor = masinf;
      // Obtengo los hijos del nodo actual y los recorro
      ParchisBros rama = actual.getChildren();
      for (ParchisBros::Iterator it = rama.begin(); it != rama.end(); ++it)
      {
         Parchis nuevo_hijo = *it;
         // Verificar si hemos alcanzado el límite
         if (NodeCounter::isLimitReached())
         {
            cout << "Límite de nodos alcanzado, devolviendo el mejor nodo parcial" << endl;
            return heuristic->evaluate(actual, jugador);
         }
         // Búsqueda en profundidad (llamada recursiva)
         float new_val = Minimax_Limitado(nuevo_hijo, jugador, profundidad + 1, profundidad_max, c_piece,
         id_piece, dice, heuristic);
         // Me voy quedando con el mínimo
         if (new_val < valor)
         {
            valor = new_val;
         }
      }
      return valor;
   }
}

// Poda Alfa-Beta. Mismo resultado que Minimax pero sin explorar ramas que no pueden mejorar
// la mejor opción ya conocida: alpha es lo mínimo que tiene asegurado MAX y beta lo máximo que
// le dejaría MIN. En cuanto alpha >= beta, el resto de hermanos se poda.
// También respeta el límite de nodos.
float AIPlayer::Poda_AlfaBeta(const Parchis &actual, int jugador, int profundidad, int profundidad_max, color &c_piece, int &id_piece, int &dice, float alpha, float beta, Heuristic *heuristic) const
{
   if ( actual.gameOver() || profundidad == profundidad_max )
   {
      return heuristic->evaluate(actual, jugador);
   }

   if (actual.getCurrentPlayerId() == jugador)
   {
      float valor = menosinf;
      ParchisBros rama = actual.getChildren();

      for (ParchisBros::Iterator it = rama.begin(); it != rama.end(); ++it)
      {
         Parchis nodo_hijo = *it;

         if (NodeCounter::isLimitReached())
         {
            cout << "Límite de nodos alcanzado, devolviendo el mejor nodo parcial" << endl;
            if (profundidad == 0)
            {
               c_piece = it.getMovedColor();
               id_piece = it.getMovedPieceId();
               dice = it.getMovedDiceValue();
            }
            return valor;
         }

         float nuevo_valor = Poda_AlfaBeta(nodo_hijo, jugador, profundidad + 1, profundidad_max, c_piece, id_piece, dice, alpha, beta, heuristic);
         if (valor < nuevo_valor)
         {
            valor = nuevo_valor;
            if (profundidad == 0)
            {
               c_piece = it.getMovedColor();
               id_piece = it.getMovedPieceId();
               dice = it.getMovedDiceValue();
            }
            alpha = valor;
         }

         if (alpha >= beta) break;
      }
      return valor;
   }
   else
   {
      float valor = masinf;
      ParchisBros rama = actual.getChildren();

      for (ParchisBros::Iterator it = rama.begin(); it != rama.end(); ++it)
      {
         Parchis nodo_hijo = *it;

         if (NodeCounter::isLimitReached())
         {
            cout << "Límite de nodos alcanzado, devolviendo el mejor nodo parcial" << endl;
            if (profundidad == 0)
            {
               c_piece = it.getMovedColor();
               id_piece = it.getMovedPieceId();
               dice = it.getMovedDiceValue();
            }
            return valor;
         }

         float nuevo_valor = Poda_AlfaBeta(nodo_hijo, jugador, profundidad + 1, profundidad_max, c_piece, id_piece, dice, alpha, beta, heuristic);

         if (valor > nuevo_valor)
         {
            valor = nuevo_valor;
            if (profundidad == 0)
            {
               c_piece = it.getMovedColor();
               id_piece = it.getMovedPieceId();
               dice = it.getMovedDiceValue();
            }
            beta = valor;
         }
         if (alpha >= beta) break;
      }
      return valor;
   }
}

// Poda Alfa-Beta con profundidad dinámica. Si un nodo tiene pocos hijos (menos de 4), la jugada está
// muy condicionada y cuesta poco mirar más lejos, así que amplío en 1 el límite de profundidad para esa rama.
// Con muchos hijos se mantiene el límite normal. Es la versión que mejor me ha funcionado.
float AIPlayer::Poda_AlfaBeta_Dinamica(const Parchis& actual, int jugador, int profundidad, int profundidad_max, color& c_piece, int& id_piece, int& dice, float alpha, float beta, Heuristic* heuristic) const
{
    if (actual.gameOver() || profundidad == profundidad_max)
    {
        return heuristic->evaluate(actual, jugador);
    }

    vector<ParchisSis> hijos = actual.getChildrenList();
    const int numeroDeHijos = hijos.size();

    int limite = profundidad_max;
    if (numeroDeHijos < 4) limite++;

    if (actual.getCurrentPlayerId() == jugador)
    {

        float valor = menosinf;
        for (int i = 0; i < numeroDeHijos; ++i)
        {
            if (NodeCounter::isLimitReached())
            {
                cout << "Límite de nodos alcanzado, devolviendo el mejor nodo parcial" << endl;

                if (profundidad == 0)
                {
                    c_piece = hijos[i].getMovedColor();
                    id_piece = hijos[i].getMovedPieceId();
                    dice = hijos[i].getMovedDiceValue();
                }
                return valor;
            }

            Parchis nuevo_hijo = *hijos[i];
            float nuevo_valor = Poda_AlfaBeta_Dinamica(nuevo_hijo, jugador, profundidad + 1, limite, c_piece, id_piece, dice, alpha, beta, heuristic);

            if (nuevo_valor > valor)
            {
                valor = nuevo_valor;
                if (profundidad == 0)
                {
                    c_piece = hijos[i].getMovedColor();
                    id_piece = hijos[i].getMovedPieceId();
                    dice = hijos[i].getMovedDiceValue();
                }
                alpha = valor;
            }

            if (alpha >= beta) break;
        }
        return valor;
    }
    else
    {

        float valor = masinf;
        for (int i = 0; i < numeroDeHijos; ++i)
        {
            if (NodeCounter::isLimitReached())
            {
                cout << "Límite de nodos alcanzado, devolviendo el mejor nodo parcial" << endl;
                if (profundidad == 0)
                {
                    c_piece = hijos[i].getMovedColor();
                    id_piece = hijos[i].getMovedPieceId();
                    dice = hijos[i].getMovedDiceValue();
                }
                return valor;
            }

            Parchis nuevo_hijo = *hijos[i];
            float nuevo_valor = Poda_AlfaBeta_Dinamica(nuevo_hijo, jugador, profundidad + 1, limite, c_piece, id_piece, dice, alpha, beta, heuristic);

            if (valor > nuevo_valor)
            {
                valor = nuevo_valor;
                if (profundidad == 0)
                {
                    c_piece = hijos[i].getMovedColor();
                    id_piece = hijos[i].getMovedPieceId();
                    dice = hijos[i].getMovedDiceValue();
                }
                beta = valor;
            }

            if (alpha >= beta) break;
        }
        return valor;
    }
}

// Ordena de mayor a menor valor heurístico (para los nodos MAX)
bool comparar(const pair<float, ParchisBros::Iterator> &p1, const pair<float, ParchisBros::Iterator> &p2) {
    return p1.first > p2.first;
}

// Poda Alfa-Beta con ordenación de movimientos. Antes de expandir, evalúo cada hijo con la heurística y
// los ordeno: de mayor a menor en nodos MAX y de menor a mayor en nodos MIN. Si las mejores jugadas se
// exploran primero, alpha y beta se ajustan antes y se poda mucho más.
float AIPlayer::Poda_AlfaBetaOrdenada(const Parchis &actual, int jugador, int profundidad, int profundidad_max, color &c_piece, int &id_piece, int &dice, float alpha, float beta, Heuristic *heuristic) const
{
    if (actual.gameOver() || profundidad == profundidad_max)
    {
        return heuristic->evaluate(actual, jugador);
    }

    ParchisBros hijos = actual.getChildren();

    vector<pair<float, ParchisBros::Iterator>> ordenacion;
    for (ParchisBros::Iterator it = hijos.begin(); it != hijos.end(); ++it)
    {
       float valor_heuristica = heuristic->evaluate(*it, jugador);

        ordenacion.push_back({ valor_heuristica, it });
    }

    if (actual.getCurrentPlayerId() == jugador)
    {
        sort(ordenacion.begin(), ordenacion.end(), comparar);

        float valor = menosinf;
        for (int i = 0; i < ordenacion.size(); ++i)
        {
            Parchis hijo = *ordenacion[i].second;

            if (NodeCounter::isLimitReached())
            {
                cout << "Límite de nodos alcanzado, devolviendo el mejor nodo parcial" << endl;
                if (profundidad == 0) {
                    c_piece = ordenacion[i].second.getMovedColor();
                    id_piece = ordenacion[i].second.getMovedPieceId();
                    dice = ordenacion[i].second.getMovedDiceValue();
                }
                return valor;
            }

            float nuevo_valor = Poda_AlfaBetaOrdenada(hijo, jugador, profundidad + 1, profundidad_max, c_piece, id_piece, dice, alpha, beta, heuristic);

            if (nuevo_valor > valor)
            {
                valor = nuevo_valor;
                if (profundidad == 0)
                {
                    c_piece = ordenacion[i].second.getMovedColor();
                    id_piece = ordenacion[i].second.getMovedPieceId();
                    dice = ordenacion[i].second.getMovedDiceValue();
                }
                alpha = valor;
            }

            if (alpha >= beta) break;
        }
        return valor;
    }
    else
    {
       sort(ordenacion.begin(), ordenacion.end(), [](auto &a, auto &b) { return a.first < b.first; });

        float valor = masinf;
        for (int i = 0; i < ordenacion.size(); ++i)
        {
            Parchis hijo = *ordenacion[i].second;

            if (NodeCounter::isLimitReached())
            {
                cout << "Límite de nodos alcanzado, devolviendo el mejor nodo parcial" << endl;
                if (profundidad == 0) {
                    c_piece = ordenacion[i].second.getMovedColor();
                    id_piece = ordenacion[i].second.getMovedPieceId();
                    dice = ordenacion[i].second.getMovedDiceValue();
                }
                return valor;
            }

            float nuevo_valor = Poda_AlfaBetaOrdenada(hijo, jugador, profundidad + 1, profundidad_max, c_piece, id_piece, dice, alpha, beta, heuristic);

            if (nuevo_valor < valor)
            {
                valor = nuevo_valor;
                if (profundidad == 0)
                {
                    c_piece = ordenacion[i].second.getMovedColor();
                    id_piece = ordenacion[i].second.getMovedPieceId();
                    dice = ordenacion[i].second.getMovedDiceValue();
                }
                beta = valor;
            }

            if (alpha >= beta) break;
        }
        return valor;
    }
}

// Heurística de referencia que viene con el motor: solo cuenta fichas en casilla segura (+1) y en meta (+5).
// La uso como punto de partida y para comprobar que los algoritmos de búsqueda funcionan.
float ValoracionTest::getHeuristic(const Parchis& estado, int jugador) const{

   int ganador = estado.getWinner();
   int oponente = (jugador + 1) % 2;

   // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
   if (ganador == jugador){
      return gana;
   }
   else if (ganador == oponente){
      return pierde;
   }
   else{
      // Colores que juega mi jugador y colores del oponente
      vector<color> my_colors = estado.getPlayerColors(jugador);
      vector<color> op_colors = estado.getPlayerColors(oponente);

      // Recorro todas las fichas de mi jugador
      int puntuacion_jugador = 0;
      // Recorro colores de mi jugador.
      for (int i = 0; i < my_colors.size(); i++){
         color c = my_colors[i];
         // Recorro las fichas de ese color.
         for (int j = 0; j < num_pieces; j++){
            // Valoro positivamente que la ficha esté en casilla segura o meta.
            if (estado.isSafePiece(c, j)){
               puntuacion_jugador++;
            }
            else if (estado.getBoard().getPiece(c, j).get_box().type == goal){
               puntuacion_jugador += 5;
            }
         }
      }

      // Recorro todas las fichas del oponente
      int puntuacion_oponente = 0;
      // Recorro colores del oponente.
      for (int i = 0; i < op_colors.size(); i++){
         color c = op_colors[i];
         // Recorro las fichas de ese color.
         for (int j = 0; j < num_pieces; j++){
            if (estado.isSafePiece(c, j)){
               // Valoro negativamente que la ficha esté en casilla segura o meta.
               puntuacion_oponente++;
            }
            else if (estado.getBoard().getPiece(c, j).get_box().type == goal){
               puntuacion_oponente += 5;
            }
         }
      }

      // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
      return puntuacion_jugador - puntuacion_oponente;
   }
}

// Valoracion1: avance de cada ficha hacia la meta, premio por fichas en meta y castigo por fichas en casa.
// Es simétrica: lo que suma para mí lo resta para el rival (con pesos ligeramente distintos).
float Valoracion1::getHeuristic(const Parchis& estado, int jugador) const {

   if (estado.gameOver())
   {
      if (estado.getWinner() == jugador)
      {
         return gana;
      } else
      {
         return pierde;
      }
   }

   float total = 0;
   int rival;

   rival = (jugador == 0) ? 1 : 0;
   vector<color> coloresPropios = estado.getPlayerColors(jugador);
   vector<color> coloresRival = estado.getPlayerColors(rival);

   auto Comienza_La_Cuenta = [&](const vector<color>& coloresJugador, bool activado)
   {
      float signo_avance = activado ? 1 : -1;
      float peso_meta = activado ? 18 : -17;
      float peso_casa = activado ? -13 : 12;

      for (int i = 0 ; i < coloresJugador.size(); i++)
      {
         color c = coloresJugador[i];
         total += peso_meta * estado.piecesAtGoal(c);
         total += peso_casa * estado.piecesAtHome(c);

         for (int j = 0; j < num_pieces; ++j)
         {
            int avance = CASILLAS - estado.distanceToGoal(c, j);
            total += signo_avance * avance;
         }
      }
   };

   Comienza_La_Cuenta(coloresPropios, true);
   Comienza_La_Cuenta(coloresRival, false);

   return total;
}

// Valoracion2: Valoracion1 + premio por fichas en casilla segura y en el pasillo final hacia la meta.
float Valoracion2::getHeuristic(const Parchis& estado, int jugador) const {

   if (estado.gameOver())
   {
      if (estado.getWinner() == jugador)
      {
         return gana;
      } else
      {
         return pierde;
      }
   }

   float total = 0;
   int rival;

   rival = (jugador == 0) ? 1 : 0;
   vector<color> colorDelGanador = estado.getPlayerColors(jugador);
   vector<color> colorDelRival   = estado.getPlayerColors(rival);

   auto Comienza_La_Cuenta = [&](const vector<color>& coloresJugador, bool activado)
   {
      float signo_avance = activado ? 1 : -1;
      float premio_meta  = activado ? 18 : -17;
      float penalizacion_casa = activado ? -13 : 12;
      float premio_seguridad   = activado ? 4  : -4.5;
      float premio_cola        = activado ? 6.5  : -7.5;

      for (int i = 0; i < coloresJugador.size(); ++i)
      {
         color c = coloresJugador[i];
         total += premio_meta * estado.piecesAtGoal(c);
         total += penalizacion_casa * estado.piecesAtHome(c);

         for (int j = 0; j < num_pieces; ++j)
         {
            Piece ficha = estado.getBoard().getPiece(c, j);
            int avance = CASILLAS - estado.distanceToGoal(c, j);
            total += signo_avance * avance;

            if (estado.isSafePiece(c, j))
            {
               total += premio_seguridad;
            }

            if (ficha.get_box().type == final_queue)
            {
               total += premio_cola;
            }
         }
      }
   };

   Comienza_La_Cuenta(colorDelGanador, true);   // Mis fichas
   Comienza_La_Cuenta(colorDelRival, false);    // Fichas del rival

   return total;
}

// Valoracion3: Valoracion2 + eventos del último movimiento (comer, entrar en meta, rebotar),
// distinguiendo si han ocurrido en mi turno o en el del rival.
float Valoracion3::getHeuristic(const Parchis& estado, int jugador) const {

   if (estado.gameOver())
   {
      if (estado.getWinner() == jugador)
      {
         return gana;
      } else
      {
         return pierde;
      }
   }

   float total = 0;
   int rival = (jugador == 0) ? 1 : 0;
   bool turno = (estado.getCurrentPlayerId() == jugador);

   vector<color> colorDelGanador = estado.getPlayerColors(jugador);
   vector<color> colorDelRival   = estado.getPlayerColors(rival);

   if (estado.isEatingMove())
   {
      pair<color, int> PiezaComida = estado.eatenPiece();
      bool PiezaComidaEraMia = find(colorDelGanador.begin(), colorDelGanador.end(), PiezaComida.first) != colorDelGanador.end();

      if (turno)
      {
         total += PiezaComidaEraMia ? -6 : 60;
      }
      else
      {
         total += PiezaComidaEraMia ? -65 : 4;
      }
   }

   if (estado.isGoalMove())
   {
      total += turno ? 30 : -30;
   }

   if (estado.goalBounce())
   {
      total -= 2.0f;
   }

   auto Comienza_La_Cuenta = [&](const vector<color>& coloresJugador, bool activado)
   {
      float signo_avance = activado ? 1 : -1;
      float premio_meta  = activado ? 18 : -17.5;
      float premio_cola  = activado ? 6.5  : -7.5;
      float premio_seguridad = activado ? 4 : -4.5;
      float penalizacion_casa = activado ? -13 : 12;

      for (int i = 0; i < coloresJugador.size(); ++i)
      {
         color c = coloresJugador[i];
         total += premio_meta * estado.piecesAtGoal(c);
         total += penalizacion_casa * estado.piecesAtHome(c);

         for (int j = 0; j < num_pieces; ++j)
         {
            Piece ficha = estado.getBoard().getPiece(c, j);
            int avance = CASILLAS - estado.distanceToGoal(c, j);
            total += signo_avance * avance;

            if (estado.isSafePiece(c, j))
            {
               total += premio_seguridad;
            }
            if (ficha.get_box().type == final_queue)
            {
               total += premio_cola;
            }
         }
      }
   };

   Comienza_La_Cuenta(colorDelGanador, true);   // Mis fichas
   Comienza_La_Cuenta(colorDelRival, false);    // Fichas del rival

   return total;
}

// MejorValoracion (definitiva): todo lo de Valoracion3 + penalización por barreras en el camino a la meta.
// Una barrera de mi compañero estorba poco; una del rival, bastante más.
float MejorValoracion::getHeuristic(const Parchis& estado, int jugador) const {

   if (estado.gameOver())
   {
      if (estado.getWinner() == jugador)
      {
         return gana;
      } else
      {
         return pierde;
      }
   }

   float total = 0;
   int rival;
   bool turno;


   rival = (jugador == 0) ? 1 : 0;
   turno = (estado.getCurrentPlayerId() == jugador);
   vector<color> colorDelGanador = estado.getPlayerColors(jugador);
   vector<color> colorDelRival = estado.getPlayerColors(rival);


    // Comprueba si ha entrado a la meta
    if (estado.isGoalMove())
    {
      total += turno ? 30 : -30;
    }

    // Comprueba si ha rebotado al entrar a la meta
    if (estado.goalBounce())
    {
      total -= 2;
    }
    // Comprueba si se ha comido una ficha
    if (estado.isEatingMove())
    {
        pair<color, int> PiezaComida = estado.eatenPiece();
        bool PiezaComidaEraMia = find(colorDelGanador.begin(), colorDelGanador.end(), PiezaComida.first) != colorDelGanador.end();

        if (turno)
        {
           total += PiezaComidaEraMia ? -6 : 60;
        }
        else
        {
          total += PiezaComidaEraMia ? -65 : 4;
        }
    }

   // Lambda que puntúa todas las fichas de unos colores; activado = true para los míos, false para los del rival
   auto Comienza_La_Cuenta = [&](const vector<color>& coloresJugador, bool activado)
   {
      float signo_avance = activado ? 1 : -1;
      float premio_meta = activado ? 18 : -17.5;
      float premio_cola = activado ? 6.5 : -7.5;
      float premio_seguridad = activado ? 4 : -4.5;
      float penalizacion_casa = activado ? -13 : 12;
      float penalizacion_barrera_mia = -2;
      float penalizacion_barrera_enemigo = -12.5;

      for (int i = 0; i < coloresJugador.size(); ++i)
      {
         color c = coloresJugador[i];
         Box obj = estado.getFinalBox(c);
         color partner = partner_color(c);

         total += premio_meta * estado.piecesAtGoal(c);
         total += penalizacion_casa * estado.piecesAtHome(c);

         for (int j = 0; j < num_pieces; ++j)
         {
            Piece ficha = estado.getBoard().getPiece(c, j);
            int avance = CASILLAS - estado.distanceToGoal(c, j);
            total += signo_avance * avance;

            if (estado.isSafePiece(c, j))
               total += premio_seguridad;

            if (ficha.get_box().type == final_queue)
               total += premio_cola;

            vector<color> bloqueos = estado.anyWall(ficha.get_box(), obj);
            for (int k = 0; k < bloqueos.size(); ++k)
            {
               color color_ficha_bloqueando = bloqueos[k];
               if (color_ficha_bloqueando == partner)
               {

                  total += penalizacion_barrera_mia * signo_avance;
               }
               else
               {
                  total += penalizacion_barrera_enemigo * signo_avance;
               }
            }
         }
      }
   };

    // Evaluamos primero mis fichas
    Comienza_La_Cuenta(colorDelGanador, true);

   // Evaluamos después las fichas del rival
    Comienza_La_Cuenta(colorDelRival, false);

    return total;
}
