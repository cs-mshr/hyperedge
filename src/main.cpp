#include "crow.h"
#include <cpr/cpr.h>

int main()
{
    crow::SimpleApp app;
    std::string URL = "https://test.deribit.com/api/v2/";
    std::string access_token;

    CROW_ROUTE(app, "/health")([](){
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

    /*
     * TODO: add support for all parameters present in API-DOCS
    */
    CROW_ROUTE(app, "/edit").methods(crow::HTTPMethod::POST)
    ([&URL, &access_token](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) {
            return crow::response(400, "Invalid JSON");
        }

        std::string order_id = body["order_id"].s();
        std::string amount = body.has("amount") ? body["amount"].s() : std::string();
        std::string price = body.has("price") ? body["price"].s() : std::string();
        std::string advanced = body.has("advanced") ? body["advanced"].s() : std::string();

        if (order_id.empty()) {
            return crow::response(400, "Missing order_id parameter");
        }

        cpr::Parameters params{{"order_id", order_id}};
        if (!amount.empty()) params.Add({"amount", amount});
        if (!price.empty()) params.Add({"price", price});
        if (!advanced.empty()) params.Add({"advanced", advanced});

        std::string url = URL + "private/edit";

        cpr::Response response = cpr::Get(
            cpr::Url{url},
            params,
            cpr::Header{{"Authorization", "Bearer " + access_token},
                        {"Content-Type", "application/json"}}
        );

        if (response.status_code == 200) {
            return crow::response("Order edited successfully. API Response: " + response.text);
        } else {
            return crow::response(500, "Failed to edit order. API Response: " + response.text);
        }
    });


    CROW_ROUTE(app, "/cancel").methods(crow::HTTPMethod::POST)
    ([&URL, &access_token](const crow::request& req) {

        std::string url = URL + "private/cancel";

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
            return crow::response(500, "Failed to cancel order. API Response: " + response.text);
        }

    });

    CROW_ROUTE(app, "/cancel_all").methods(crow::HTTPMethod::POST)
    ([&URL, &access_token](const crow::request& req) {

        std::string url = URL + "private/cancel_all";

        cpr::Response response = cpr::Get(
            cpr::Url{url},
            cpr::Header{{"Authorization", "Bearer " + access_token},
                        {"Content-Type", "application/json"}
            });

        if (response.status_code == 200) {
            return crow::response(response.text);
        } else {
            return crow::response(500, "Failed to cancel ALL orders. API Response: " + response.text);
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
            return crow::response(500, "Failed to get open order details. API Response: " + response.text);
        }

    });

    /*
     * TODO: add depth to api call if provided in req_body.
     */
    CROW_ROUTE(app, "/get_order_book").methods(crow::HTTPMethod::POST)
    ([&URL](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if(!body) {
            return crow::response(400, "Invalid JSON");
        }

        std::string instrument_name = body["instrument_name"].s();
        if (instrument_name.empty()) {
            return crow::response(400, "Missing instrument_name parameter");
        }

        std::string url = URL + "public/get_order_book";

        cpr::Response response = cpr::Get(
            cpr::Parameters{{"instrument_name", instrument_name}},
            cpr::Url{url},
            cpr::Header{{"Content-Type", "application/json"}}
        );

        if (response.status_code == 200) {
            return crow::response(response.text);
        } else {
            return crow::response(500, "Failed to get order book. API Response: " + response.text);
        }
    });

    CROW_ROUTE(app, "/get_position").methods(crow::HTTPMethod::POST)
    ([&URL, &access_token](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) {
            return crow::response(400, "Invalid JSON");
        }

        std::string instrument_name = body["instrument_name"].s();
        if (instrument_name.empty()) {
            return crow::response(400, "Missing instrument_name parameter");
        }

        std::string url = URL + "private/get_position";

        cpr::Response response = cpr::Get(
            cpr::Url{url},
            cpr::Parameters{{"instrument_name", instrument_name}},
            cpr::Header{{"Authorization", "Bearer " + access_token},
                        {"Content-Type", "application/json"}}
        );

        if (response.status_code == 200) {
            return crow::response(response.text);
        } else {
            return crow::response(500, "Failed to retrieve position. API Response: " + response.text);
        }
    });

    app.bindaddr("127.0.0.1").port(18080).multithreaded().run();
}