#pragma once
#include <gtest/gtest.h>
#include <string_view>
#include <format>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <charconv>
#include "Orderbook.hpp"



enum class ActionType{
    Add,
    Modify,
    Cancel,
};

struct Info{
    ActionType type_;
    OrderType orderType_;
    Side side_;
    Price price_;
    Quantity quantity_;
    OrderId orderId_;
};

using Infos = std::vector<Info>;

struct Result{
    std::size_t allCount_;
    std::size_t bidCount_;
    std::size_t askCount_;
};

/**
 * @class InputHandle
 * @brief parsing and processing of input files related to orders.
 */
struct InputHandle{
    private:
        /**
         * @brief Convert string to a number.
         * @param str String to convert.
         * @return Converted number as a uint64_t.
         * @throws std::logic_error if the value is negative.
         */
        std::uint64_t ToNumber(const std::string_view& str) const{
            std::int64_t value{};
            std::from_chars(str.data(),str.data()+str.size(),value);
            if(value<0)
                throw std::logic_error("Value is below zero.");
            return static_cast<std::uint64_t>(value);
        }
        /**
         * @brief Parse a result line from the input file.
         * @param str Line to parse.
         * @param result Result struct to populate.
         * @return T/F
         */
        bool AttemptResult(const std::string_view& str, Result& result) const{
            if(str.at(0)!= 'R')
                return false;

            auto values = Split(str, ' '); //R 0 0 0
            result.allCount_ = ToNumber(values.at(1));
            result.bidCount_ = ToNumber(values.at(2));
            result.askCount_ = ToNumber(values.at(3));

            return true;

        }

        /**
         * @brief Attempt parse update lines from the input file.
         * @param str Line to parse.
         * @param info Info struct to populate.
         * @return T/F
         */
        bool AttemptInfo(const std::string_view& str, Info& info) const{
            auto value = str.at(0);
            auto values = Split(str, ' ');
            if(value == 'A'){
                info.type_ = ActionType::Add;
                info.side_ = PassSide(values.at(1));
                info.orderType_ = PassOrderType(values.at(2));
                info.price_ = PassPrice(values.at(3));
                info.quantity_ = PassQuantity(values.at(4));
                info.orderId_ = PassOrderId(values.at(5));
            }
            else if(value == 'M'){
                info.type_ = ActionType::Modify;
                info.orderId_ = PassOrderId(values.at(1));
                info.side_ = PassSide(values.at(2));
                info.price_ = PassPrice(values.at(3));
                info.quantity_ = PassQuantity(values.at(4));
                
            }
            else if (value == 'C'){
                info.type_ = ActionType::Cancel;
                info.orderId_ = PassOrderId(values.at(1));
            }
            else return false;

            return true;

        }

        /**
         * @brief Split str into a vector of substrs based on a delimiter.
         * @param str Str to split.
         * @param delimiter Character to use as the delimiter (A,M,C)
         * @return A vector of substrings.
         */
        std::vector<std::string_view> Split(const std::string_view& str, char delimeter) const{
            std::vector<std::string_view> columns{};
            std::size_t startIndex{}, endIndex{};
            while((endIndex=str.find(delimeter,startIndex)) && endIndex != std::string::npos){
                auto distance = endIndex - startIndex;
                auto column = str.substr(startIndex,distance);
                startIndex = endIndex +1;
                columns.push_back(column);
            }
            columns.push_back(str.substr(startIndex));
            return columns;
        }

        /**
         * @brief Parses a str to determine the order side.
         * @param str Str to parse.
         * @return Side of the order.
         * @throws std::logic_error if the side is unknown.
         */
        Side PassSide(const std::string_view& str) const{
            if(str == "B")
                return Side::Buy;
            else if (str == "S")
                return Side::Sell;
            else throw std::logic_error("Unknown Side");
        }

        /**
         * @brief Parses a str to determine the order type.
         * @param str Str to parse.
         * @return Type of the order.
         * @throws std::logic_error if the order type is unknown.
         */
        OrderType PassOrderType(const std::string_view& str) const{
            if(str == "FillAndKill")
                return OrderType::FillAndKill;
            else if(str == "GoodTillCancel")
                return OrderType::GoodTillCancel;
            else if(str == "FillOrKill")
                return OrderType::FillOrKill;
            else if(str == "Market")
                return OrderType::Market;
            else throw std::logic_error("Unknown OrderType");
        }

        /**
         * @brief Parses a str to determine the order price.
         * @param str Str to parse.
         * @return Price of the order.
         * @throws std::logic_error if the price is unknown.
         */
        Price PassPrice(const std::string_view& str) const{
            if(str.empty())
                throw std::logic_error("Unknown Price");
            return ToNumber(str);
        }

        /**
         * @brief Parses a str to identify quantity of an order.
         * @param str Str to parse.
         * @return Quantity of the order.
         * @throws std::logic_error if the quantity is unknown.
         */
        Quantity PassQuantity(const std::string_view& str) const{
            if(str.empty())
                throw std::logic_error("Unknown Quantity");
            return ToNumber(str);
        }

        /**
         * @brief Parses a str to find an order ID.
         * @param str Str to parse.
         * @return ID of the order.
         * @throws std::logic_error if the quantity is unknown.
         */
        OrderId PassOrderId(const std::string_view& str) const{
            if(str.empty())
                throw std::logic_error("Unknown OrderId");
            return ToNumber(str);
        }

    public:
        /**
         * @brief Read & parses the input file to extract info and results.
         * @param path The path to the input file.
         * @return A tuple containing a vector of Info structs and a `Result` struct.
         * @throws std::logic_error if the result is unspecified or the file format is incorrect.
         */
        std::tuple<Infos,Result> GetInfos(const std::filesystem::path& path) const{
            Infos infos;
            infos.reserve(1'000);

            std::string line;
            std:: ifstream file{path};
            while(std::getline(file,line)){
                if(line.empty())
                    break;

                const bool isResult = line.at(0) == 'R';
                const bool isUpdate = !isResult;

                if (isUpdate){
                    Info update;
                    auto isValid = AttemptInfo(line, update);
                    if(!isValid)
                        throw std::logic_error(std::format("Invalid update: {}", line));
                    
                    infos.push_back(update);
                }
                else{
                    if(!file.eof())
                        throw std::logic_error("Result must be at the end of the file only.");

                    Result result;

                    auto isValid = AttemptResult(line,result);
                    if(!isValid)
                        continue;

                    return {infos, result};

                }
            }
            throw std::logic_error("Result unspecified.");
        }
};
