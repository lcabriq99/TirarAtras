#include "functions.h"
#include <iostream>
#include <sstream>
#include <thread> 

Player parseInitialMessage(std::string &message, Player &player)
{
    std::string delimiter = " ";
    size_t pos = 0;
    std::string token;
    int i = 0;
    while ((pos = message.find(delimiter)) != std::string::npos)
    {
        token = message.substr(0, pos);
        if (i == 1)
        {
            player.side = token;
        }
        else if (i == 2)
        {
            player.unum = std::stoi(token);
        }
        else if (i == 3)
        {
            player.playmode = token;
        }
        message.erase(0, pos + delimiter.length());
        i++;
    }
    cout << "Player: " << player.side << " " << player.unum << " " << player.playmode << endl;
    return player;
}

vector<string> separate_string_separator(string &s, string separator)
{
    vector<string> v;
    int pos = 0;
    while ((pos = s.find(separator)) != string::npos)
    {
        v.push_back(s.substr(0, pos));
        s.erase(0, pos + separator.length());
    }
    v.push_back(s);
    return v;
}

vector<string> separate_string(string &s)
{
    vector<string> v;
    int pos{0};
    int level{0};
    string temp;

    while (pos < s.size())
    {
        if (s[pos] == '(')
        {
            level++;
        }
        else if (s[pos] == ')')
        {
            level--;
        }

        if (s[pos] == '(' && level == 1)
        {
            temp = "";
        }
        else if (s[pos] == ')' && level == 0)
        {
            v.push_back(temp);
        }
        else
        {
            temp += s[pos];
        }
        pos++;
    }

    if (level != 0)
    {
        throw std::runtime_error("No se han abierto y cerrado los mismos parentesis");
    }
    else
    {
        return v;
    }
}

void store_data_see(vector<string> &see_message, Player &player, Ball &ball, Goal &own_goal, Goal &opponent_goal, Field &field)
{

    // Depuración: Imprimir el mensaje see completo
    /*
    cout << "Depuracion Inicio: " << endl;
    cout << "Mensaje see: " << endl;
    for (const auto &msg : see_message)
    {
        cout << msg << endl;
    }
    cout << "Depuracion Final : " << endl;
    cout << "" << endl;
    */


    vector<string> ball_coords;
    bool found_ball = false;
    player.flags_seen = 0;
    player.see_ball = false;
    player.see_own_goal = false;
    player.see_opponent_goal = false;

    for (size_t i = 0; i < see_message.size(); i++)
    {
        if (see_message[i].find("(b)") != string::npos)
        {
            player.see_ball = true;
            ball_coords = separate_string_separator(see_message[i], " ");
            ball.x = ball_coords[1];
            ball.y = ball_coords[2];
            double distance = sqrt(pow(stod(ball.x), 2) + pow(stod(ball.y), 2));
            ball.distance = distance;
            double angle = atan2(stod(ball.y), stod(ball.x));
            ball.angle = angle * 180 / M_PI;
            found_ball = true;
        }

        if (see_message[i].find("(g r)") != string::npos)
        {
            vector<string> goal_coords = separate_string_separator(see_message[i], " ");

            if (own_goal.side == "r")
            {
                own_goal.x = goal_coords[2];
                own_goal.y = goal_coords[3];
                own_goal.angle = atan2(stof(goal_coords[3]), stof(goal_coords[2])) * 180 / M_PI;
                own_goal.distance = sqrt(pow(stof(goal_coords[2]), 2) + pow(stof(goal_coords[3]), 2));
                player.see_own_goal = true;
            }
            else
            {
                opponent_goal.x = goal_coords[2];
                opponent_goal.y = goal_coords[3];
                opponent_goal.angle = atan2(stof(goal_coords[3]), stof(goal_coords[2])) * 180 / M_PI;
                opponent_goal.distance = sqrt(pow(stof(goal_coords[2]), 2) + pow(stof(goal_coords[3]), 2));
                player.see_opponent_goal = true;
            }
        }

        if (see_message[i].find("(g l)") != string::npos)
        {
            vector<string> goal_coords = separate_string_separator(see_message[i], " ");

            if (own_goal.side == "l")
            {
                own_goal.x = goal_coords[2];
                own_goal.y = goal_coords[3];
                own_goal.distance = sqrt(pow(stof(goal_coords[2]), 2) + pow(stof(goal_coords[3]), 2));
                own_goal.angle = atan2(stof(goal_coords[3]), stof(goal_coords[2])) * 180 / M_PI;
                player.see_own_goal = true;
            }
            else
            {
                opponent_goal.x = goal_coords[2];
                opponent_goal.y = goal_coords[3];
                opponent_goal.angle = atan2(stof(goal_coords[3]), stof(goal_coords[2])) * 180 / M_PI;
                opponent_goal.distance = sqrt(pow(stof(goal_coords[2]), 2) + pow(stof(goal_coords[3]), 2));
                player.see_opponent_goal = true;
            }
        }

        if (see_message[i].find("(f c)") != string::npos)
        {
            vector<string> center_coords = separate_string_separator(see_message[i], " ");
            field.flag_center = {stof(center_coords[2]), stof(center_coords[3])};
            field.flag_center_distance = sqrt(pow(stof(center_coords[2]), 2) + pow(stof(center_coords[3]), 2));
        }

        if (see_message[i].find("(f c t)") != string::npos)
        {
            vector<string> center_top_coords = separate_string_separator(see_message[i], " ");
            field.flag_center_top = {stof(center_top_coords[3]), stof(center_top_coords[4])};
            field.flag_center_top_distance = sqrt(pow(stof(center_top_coords[3]), 2) + pow(stof(center_top_coords[4]), 2));
            player.flags_seen++;
        }

        if (see_message[i].find("(f c b)") != string::npos)
        {
            vector<string> center_bottom_coords = separate_string_separator(see_message[i], " ");
            field.flag_center_bottom = {stof(center_bottom_coords[3]), stof(center_bottom_coords[4])};
            field.flag_center_bottom_distance = sqrt(pow(stof(center_bottom_coords[3]), 2) + pow(stof(center_bottom_coords[4]), 2));
            player.flags_seen++;
        }

        if (see_message[i].find("(f l t)") != string::npos)
        {
            vector<string> left_top_coords = separate_string_separator(see_message[i], " ");
            field.flag_left_top = {stof(left_top_coords[3]), stof(left_top_coords[4])};
            field.flag_left_top_distance = sqrt(pow(stof(left_top_coords[3]), 2) + pow(stof(left_top_coords[4]), 2));
            field.flag_left_top_angle = atan2(stof(left_top_coords[4]), stof(left_top_coords[3])) * 180 / M_PI;
            player.flags_seen++;
            player.see_left_top = true;
        }

        if (see_message[i].find("(f l b)") != string::npos)
        {
            vector<string> left_bottom_coords = separate_string_separator(see_message[i], " ");
            field.flag_left_bottom = {stof(left_bottom_coords[3]), stof(left_bottom_coords[4])};
            field.flag_left_bottom_distance = sqrt(pow(stof(left_bottom_coords[3]), 2) + pow(stof(left_bottom_coords[4]), 2));
            player.flags_seen++;
        }

        if (see_message[i].find("(f r b)") != string::npos)
        {
            vector<string> right_bottom_coords = separate_string_separator(see_message[i], " ");
            field.flag_right_bottom = {stof(right_bottom_coords[3]), stof(right_bottom_coords[4])};
            field.flag_right_bottom_distance = sqrt(pow(stof(right_bottom_coords[3]), 2) + pow(stof(right_bottom_coords[4]), 2));
            player.flags_seen++;
        }
        
    }
    if (found_ball == false)
    {
        player.see_ball = false;
    }
}

void chutarPorteria(Player &player, Ball &ball, Goal &opponent_goal, MinimalSocket::udp::Udp<true> &udp_socket, MinimalSocket::Address const &server_udp)
{
    double angle_to_goal = atan2(stod(opponent_goal.y) - player.y, stod(opponent_goal.x) - player.x) * 180 / M_PI;
    int power = 100; // Potencia del chute
    std::string kick_command = "(kick " + to_string(power) + " " + to_string(angle_to_goal) + ")";
    udp_socket.sendTo(kick_command, server_udp); // Enviar comando de chute
}



float calcularDistanciaJugadorBalon(const Player &jugador, const Ball &balon)
{
    return sqrt(pow(stof(balon.x) - jugador.x, 2) + pow(stof(balon.y) - jugador.y, 2));
}

JugadorCercano procesarJugadoresVisibles(const vector<string> &see_message, const Player &player)
{
    JugadorCercano jugador_mas_cercano;
    float menor_distancia = std::numeric_limits<float>::max();

    for (auto &obj : see_message)
    {
        if (obj.find("(p") != string::npos && obj.find("(p)") == string::npos)
        {
            string obj_copy = obj; // Hacemos una copia local del string
            vector<string> player_info = separate_string_separator(obj_copy, " ");
            if (player_info.size() < 5) continue; // Asegurar que hay suficientes elementos

            JugadorCercano jugador;
            jugador.nombreEquipo = player_info[1];
            jugador.dorsal = player_info[2];
            jugador.distancia = player_info[3];
            jugador.angle = stof(player_info[4]); // Convertir el ángulo a float
            jugador.distance = stof(jugador.distancia); // Convertir la distancia a float

            // Eliminar el último carácter si es un paréntesis ')'
            if (!jugador.dorsal.empty() && jugador.dorsal.back() == ')')
            {
                jugador.dorsal.pop_back();

                // Comprobar si el jugador es del mismo equipo
                if (jugador.nombreEquipo.find(player.team_name) != string::npos)
                {
                    // Verificar si es el jugador más cercano
                    if (jugador.distance < menor_distancia)
                    {
                        menor_distancia = jugador.distance;
                        jugador_mas_cercano = jugador;
                    }
                }
            }
        }
    }

    return jugador_mas_cercano;
}

void mostrarJugadorMasCercano(const JugadorCercano &jugador_mas_cercano)
{
    if (jugador_mas_cercano.dorsal.empty()) {
        std::cout << "No se encontraron jugadores cercanos." << std::endl;
        return;
    }

    std::cout << " " << std::endl;
    std::cout << "Jugador más cercano:" << std::endl;
    std::cout << "Nombre Equipo: " << jugador_mas_cercano.nombreEquipo << std::endl;
    std::cout << "Dorsal: " << jugador_mas_cercano.dorsal << std::endl;
    std::cout << "Distancia: " << jugador_mas_cercano.distancia << std::endl;
    std::cout << "Ángulo: " << jugador_mas_cercano.angle << std::endl;
    std::cout << " " << std::endl;
}






void store_data_hear(string &hear_message, Player &player, MinimalSocket::udp::Udp<true> &udp_socket, MinimalSocket::Address const &server_udp)
{
    vector<string> aux_hear_message = separate_string(hear_message); // hear 0 referee kick_off_l
    vector<string> valores_mensaje_Hear;

    for (size_t i = 0; i < aux_hear_message.size(); i++)
    {
        if (aux_hear_message[i].find("hear") != string::npos)
        {
            valores_mensaje_Hear = separate_string_separator(aux_hear_message[i], " ");
            if (valores_mensaje_Hear.size() >= 4)
            {
                cout << "TIME: " << valores_mensaje_Hear[1] << endl;
                cout << "REFEREE: " << valores_mensaje_Hear[2] << endl;
                cout << "MODO: " << valores_mensaje_Hear[3] << endl;

                string modo = valores_mensaje_Hear[3];
                handle_game_mode(modo, player, udp_socket, server_udp);
            }
            else
            {
                cout << "Error: mensaje 'hear' no tiene suficientes elementos: " << aux_hear_message[i] << " (size: " << valores_mensaje_Hear.size() << ")" << endl;
                for (size_t j = 0; j < valores_mensaje_Hear.size(); j++) {
                    cout << "Elemento " << j << ": " << valores_mensaje_Hear[j] << endl;
                }
            }
        }
    }
}

void orientarJugadorHaciaCampo(Player &player, MinimalSocket::udp::Udp<true> &udp_socket, MinimalSocket::Address const &server_udp)
{
    float target_x = 0.0;
    float target_y = 0.0;

    float angle_to_target = atan2(target_y - player.y, target_x - player.x) * 180 / M_PI;
    std::string turn_command = "(turn " + std::to_string(angle_to_target - player.angle) + ")";
    udp_socket.sendTo(turn_command, server_udp);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}




void handle_game_mode(const string &modo, Player &player, MinimalSocket::udp::Udp<true> &udp_socket, MinimalSocket::Address const &server_udp)
{
    cout << "Handle game mode: " << modo << endl;  
    if (modo == "kick_off_l" || modo == "kick_off_r")
    {
        cout << "Saque inicial: " << modo << endl;
    }
    else if (modo == "free_kick_l" || modo == "free_kick_r")
    {
        cout << "Tiro libre: " << modo << endl;
        execute_free_kick(player, udp_socket, server_udp); 
    }
    else if (modo == "goal_kick_l" || modo == "goal_kick_r")
    {
        cout << "Saque de puerta: " << modo << endl;
        execute_goal_kick(player, udp_socket, server_udp); // Portero (jugador 1)
    }
    else if (modo == "corner_kick_l" || modo == "corner_kick_r")
    {
        cout << "Saque de esquina: " << modo << endl;
        execute_corner_kick(player, udp_socket, server_udp);
    }
    else if (modo == "throw_in_l" || modo == "throw_in_r")
    {
        cout << "Saque de banda: " << modo << endl;
        execute_throw_in(player, udp_socket, server_udp);
    }
    else if (modo == "offside_l" || modo == "offside_r")
    {
        cout << "Fuera de juego: " << modo << endl;
    }
    else if (modo == "goal_l" || modo == "goal_r")
    {
        cout << "Gol: " << modo << endl;
    }
    else if (modo == "play_on")
    {
        cout << "Juego en curso" << endl;
    }
    else if (modo == "time_over")
    {
        cout << "Fin del tiempo de juego" << endl;
    }
    else
    {
        cout << "Modo desconocido: " << modo << endl;
    }
}

void execute_goal_kick(Player &player, MinimalSocket::udp::Udp<true> &udp_socket, MinimalSocket::Address const &server_udp)
{
    if (player.unum != 1) {
        cout << "Jugador " << player.unum << " no está autorizado para ejecutar saque de puerta" << endl;
        return;
    }

    std::string command = "(kick 100 0)";
    udp_socket.sendTo(command, server_udp);
    cout << "Jugador " << player.unum << " ejecuta saque de puerta con potencia 100" << endl;
}

void execute_corner_kick(Player &player, MinimalSocket::udp::Udp<true> &udp_socket, MinimalSocket::Address const &server_udp)
{
    // Orientar al jugador hacia el campo
    orientarJugadorHaciaCampo(player, udp_socket, server_udp);

    // Después de orientar al jugador, ejecutar el saque
    std::string command = "(kick 50 0)"; // Ajustar la potencia y dirección según sea necesario
    udp_socket.sendTo(command, server_udp);
    std::cout << "Jugador " << player.unum << " ejecuta saque de esquina con potencia 50 y dirección hacia el campo" << std::endl;
}

void execute_throw_in(Player &player, MinimalSocket::udp::Udp<true> &udp_socket, MinimalSocket::Address const &server_udp)
{
    // Orientar al jugador hacia el campo
    orientarJugadorHaciaCampo(player, udp_socket, server_udp);

    // Después de orientar al jugador, ejecutar el saque
    std::string command = "(kick 50 0)"; // Ajustar la potencia y dirección según sea necesario
    udp_socket.sendTo(command, server_udp);
    std::cout << "Jugador " << player.unum << " ejecuta saque de banda con potencia 50 y dirección hacia el campo" << std::endl;
}


void execute_free_kick(Player &player, MinimalSocket::udp::Udp<true> &udp_socket, MinimalSocket::Address const &server_udp)
{
    std::string command = "(kick 100 0)";
    udp_socket.sendTo(command, server_udp);
    cout << "Jugador " << player.unum << " ejecuta falta con potencia 100" << endl;
}

void noMarcarPropia(Player &player, Ball &ball, Goal &own_goal, MinimalSocket::udp::Udp<true> &udp_socket, MinimalSocket::Address const &server_udp)
{
    // Si ve la porteria de nuestro equipo intenta chutar atrás para no automarcarsee
    //Se calcula el angulo para chutar hacia porteria del equipo rivalkajh
    double angle_away_from_own_goal = atan2(player.y - stod(own_goal.y), player.x - stod(own_goal.x)) * 180 / M_PI;
    int power = 100; //Le he puesto 100 para alejar la pelota de nuestra área
    std::string kick_command = "(kick " + to_string(power) + " " + to_string(angle_away_from_own_goal) + ")";
    udp_socket.sendTo(kick_command, server_udp);
}

void saqueEsquinaBandaAtras(Player &player, MinimalSocket::udp::Udp<true> &udp_socket, MinimalSocket::Address const &server_udp)
{
    // Chutar hacia atrás (en dirección opuesta a la línea de banda)
    double angle_away_from_line = atan2(-player.y, -player.x) * 180 / M_PI;
    int power = 50; // Potencia del chute, puede ajustarse según necesidad
    std::string kick_command = "(kick " + to_string(power) + " " + to_string(angle_away_from_line) + ")";
    udp_socket.sendTo(kick_command, server_udp); // Enviar comando de chute
}
