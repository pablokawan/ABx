#pragma once

// Configurations shared by the server and client

// Email is mandatory when creating an account
//#define EMAIL_MANDATORY

// If defined disable nagle's algorithm, this make the game play smoother
// acceptor_.set_option(asio::ip::tcp::no_delay(true));
// But I think this makes some problems
//#define TCP_OPTION_NODELAY

#define PLAYER_MAX_SKILLS 8
// Most profession have 4 attribute but Warrior and Elementarist have 5
#define PLAYER_MAX_ATTRIBUTES 10
