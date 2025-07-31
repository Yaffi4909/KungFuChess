#pragma once
#include <string>
#include <iostream>
#include "../../It1_interfaces/ScoreManager.hpp"
#include "../../It1_interfaces/PubSubQueue.hpp" // Assuming PubSubQueue is defined in this path
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

void ScoreManager::on_piece_killed(const std::string &piece_id)
{
    std::cout << "Piece killed: " << piece_id << std::endl;

    char color = piece_id[1]; // צבע החייל (W, B)
    char type = piece_id[0];  // סוג החייל (P, N, B, R, Q, K)
                              // חייל = 1 פרש = 3 רץ = 3 צריח = 5 מלכה = 9

    PlaySound(L"gun.wav", NULL, SND_FILENAME | SND_ASYNC);

    send_score_update(PubSubQueue::instance(), color == 'W' ? "black" : "white", get_score_for_piece(type));

    // std::cout << "w score: " << score.first << ", b score: " << score.second << std::endl;
}

void ScoreManager::score_listener(const std::string &topic, const std::string &payload)
{
    std::cout << "Received score update (" << topic << "): " << payload << std::endl;

    size_t sep = payload.find(':');
    if (sep == std::string::npos)
    {
        std::cerr << "Invalid payload format: missing ':' in \"" << payload << "\"" << std::endl;
        return;
    }

    std::string player = payload.substr(0, sep);
    std::string score_str = payload.substr(sep + 1);

    try
    {
        int delta = std::stoi(score_str);
        std::cout << " → Player: " << player << ", Score Change: " << delta << std::endl;

        if (player == "white")
        {
            score.first += delta;
        }
        else if (player == "black")
        {
            score.second += delta;
        }
        else
        {
            std::cerr << "Unknown player: " << player << std::endl;
        }
        std::cout << " White: " << score.first << ", Black: " << score.second << std::endl;
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error parsing score from payload \"" << payload << "\": " << ex.what() << std::endl;
    }
}

void ScoreManager::send_score_update(PubSubQueue &pubsub, const std::string &player_name, int delta)
{
    std::string payload;
    try
    {
        payload = player_name + ":" + std::to_string(delta); // למשל: "white:10"
        std::cout << "Publishing score update: " << payload << std::endl;
    }
    catch (...)
    {
        std::cerr << "Failed to convert score delta to string.\n";
        return;
    }

    pubsub.publish("changeScore", payload); // פרסום לנושא המתאים
}

int ScoreManager::get_score_for_piece(const char type)
{
    if (type == 'P')
        return 1; // Pawn
    if (type == 'N')
        return 3; // Knight
    if (type == 'B')
        return 3; // Bishop
    if (type == 'R')
        return 5; // Rook
    if (type == 'Q')
        return 9; // Queen
    return 100;   // king
}
