#include "crow.h"
#include <cpr/cpr.h>

int main()
{
    crow::SimpleApp app;
    std::string URL = "https://test.deribit.com/api/v2/";
    std::string access_token;

    CROW_ROUTE(app, "/health")([&access_token](){
        return "Up!! Running...!";
    });

    CROW_ROUTE(app, "/auth").methods(crow::HTTPMethod::POST)
    ([&URL, &access_token](const crow::request& req) {

        auto body = crow::json::load(req.body);
        if(!body) {
            return crow::response(400, "Invalid JSON");
        }

        std::string client_id = body["client_id"].s();
        std::string client_secret = body["client_secret"].s();
        std::string grant_type = body["grant_type"].s();

        if (client_id.empty() || client_secret.empty() || grant_type.empty()) {
            return crow::response(400, "Missing query parameters");
        }

        std::string url = URL + "public/auth";

        cpr::Response response = cpr::Get(
            cpr::Parameters{{"client_id", client_id},
                            {"client_secret", client_secret},
                            {"grant_type", grant_type}},
            cpr::Url{url},
            cpr::Header{{"Content-Type", "application/json"}}
        );

        if (response.status_code == 200) {
            auto json_response = crow::json::load(response.text);
            access_token = json_response["result"]["access_token"].s();
            return crow::response("Access Token: " + access_token);
        } else {
            return crow::response(500, "Failed to authenticate. API Response: " + response.text);
        }
    });

    CROW_ROUTE(app, "/buy").methods(crow::HTTPMethod::POST)
    ([&URL, &access_token](const crow::request& req) {

        auto body = crow::json::load(req.body);
        if(!body) {
            return crow::response(400, "Invalid JSON");
        }

        std::string amount = body["amount"].s();
        std::string instrument_name = body["instrument_name"].s();
        std::string price = body["price"].s();

        if (amount.empty() || instrument_name.empty() || price.empty()) {
            return crow::response(400, "Missing query parameters");
        }

        std::string url = URL + "private/buy";

        cpr::Response response = cpr::Get(
            cpr::Parameters{{"amount", amount},
                            {"instrument_name", instrument_name},
                            {"price", price}},
            cpr::Url{url},
            cpr::Header{{"Authorization", "Bearer " + access_token},
                        {"Content-Type", "application/json"}}
        );

        if (response.status_code == 200) {
            return crow::response("Buy order placed successfully. API Response: " + response.text);
        } else {
            return crow::response(500, "Failed to place buy order. API Response: " + response.text);
        }
    });

    CROW_ROUTE(app, "/cancel").methods(crow::HTTPMethod::POST)
    ([&URL, &access_token](const crow::request& req) {

        std::string url = URL + "private/get_open_orders";

        auto body = crow::json::load(req.body);
        if(!body) {
            return crow::response(400, "Invalid JSON");
        }

        std::string order_id = body["order_id"].s();
        if (order_id.empty()) {
            return crow::response(400, "Missing order_id parameter");
        }

        cpr::Response response = cpr::Get(
            cpr::Url{url},
            cpr::Parameters{{"order_id", order_id}},
            cpr::Header{{"Authorization", "Bearer " + access_token},
                        {"Content-Type", "application/json"}
            });

        if (response.status_code == 200) {
            return crow::response(response.text);
        } else {
            return crow::response(500, "Failed to get order book. API Response: " + response.text);
        }

    });

    CROW_ROUTE(app, "/get_open_orders").methods(crow::HTTPMethod::GET)
    ([&URL, &access_token](const crow::request& req) {

        std::string url = URL + "private/get_open_orders";
        cpr::Response response = cpr::Get(
            cpr::Url{url},
            cpr::Header{{"Authorization", "Bearer " + access_token},
                        {"Content-Type", "application/json"}
            });

        if (response.status_code == 200) {
            return crow::response(response.text);
        } else {
            return crow::response(500, "Failed to get order book. API Response: " + response.text);
        }

    });



    app.bindaddr("127.0.0.1").port(18080).multithreaded().run();
}