#ifndef strings_h
#define strings_h

#include <string>
#include <sstream>

class Strings{
public:

    static std::string first_letter_capital(std::string str_input);

    static std::string capitalize_all_words(const std::string& str_input);

    static std::string upper_case(const std::string& str_input);

    static std::string lower_case(const std::string& str_input);

    static std::string underscore_to_space(const std::string& str_input);

    //Prepares newlines (\n) in a string to be written to disk.
    static std::string add_newlines(const std::string& str_input);

    //Process newlines (\n) in a string that has been read from disk.
    static std::string process_newlines(const std::string& str_input);

    //Returns the number of newlines in the passed string.
    static int newline_count(const std::string& str_input);

    //Returns the length of the last line in the passed string.
    //Simply returns the length of the string if it has only one line.
    static int length_of_last_line(const std::string& str_input);

    //Returns the length of the longest line in the passed string.
    static int longest_line(const std::string& str_input);

    static std::string erase_first_line(std::string str_input);

    static bool is_number(const std::string& str_input);

    //Returns a string with the time represented by seconds.
    //If highest is true, only shows the highest unit of time possible.
    static std::string time_string(int seconds,bool highest=false);

    static bool string_to_bool(const std::string& get_string);
    static std::string bool_to_string(bool get_bool);

    static long string_to_long(const std::string& get_string);
    static unsigned long string_to_unsigned_long(const std::string& get_string);
    static double string_to_double(const std::string& get_string);

    template<typename Number_Type>
    inline static std::string num_to_string(const Number_Type& number,int precision=10,bool show_point=false,std::string notation="default"){
        std::stringstream strstream("");

        strstream.precision(precision);

        if(show_point){
            strstream.setf(std::ios::showpoint);
        }

        if(notation=="fixed"){
            strstream<<std::fixed;
        }
        else if(notation=="scientific"){
            strstream<<std::scientific;
        }

        strstream<<number;

        return strstream.str();
    }

    template<typename Number_Type>
    inline static std::string num_to_roman_numeral(const Number_Type& number){
        std::string message="";

        int num=(int)number;

        if(num>=4000){
            int x=(num-num%4000)/1000;
            message="("+num_to_roman_numeral(x)+")";
            num%=4000;
        }

        const std::string roman[13]={"M","CM","D","CD","C","XC","L","XL","X","IX","V","IV","I"};
        const int decimal[13]={1000,900,500,400,100,90,50,40,10,9,5,4,1};

        for(int i=0;i<13;i++){
            while(num>=decimal[i]){
                num-=decimal[i];
                message+=roman[i];
            }
        }

        return message;
    }
};

#endif
