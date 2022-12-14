#include "program.hpp"

SourceSptr Program::source;
LexicalAnalyzerUptr Program::lexicalAnalyzer;

void Program::start(const int argc, const std::vector<std::string_view> &arguments)
{

    if (argc == 1)
    {
        startInterpreter();
    }
    else
    {
        parseFlags(arguments);
    }
}

void Program::parseFlags(const std::vector<std::string_view> &arguments)
{
    try
    {
        auto option = FlagResolver::resolveOption(arguments[1]);
        switch (option)
        {
        case (FlagResolver::Options::File):
        case (FlagResolver::Options::Socket):
        case (FlagResolver::Options::String):
            source = SourceFactory::createSource(option, arguments);
            Program::lexicalAnalyzer = std::make_unique<LexicalAnalyzer>(*source.get());
            break;
        case (FlagResolver::Options::Help):
            showHelp();
            break;
        default:
            throw WrongFlagsException("Flags you provided are invalid. Try --help for help");
            break;
        }
    }
    catch (WrongFlagsException &ex)
    {
        std::cout << ex.what() << "\n";
        return;
    }
}

void Program::showHelp()
{

    std::cout << "*******************************************************************\n";
    std::cout << "*                       AVAILABLE FLAGS:                          *\n";
    std::cout << "*   --help/-h shows help                                          *\n";
    std::cout << "*   --string/-s <source string> parse code from string            *\n";
    std::cout << "*   --file/-f <path to source file> parse code from file          *\n";
    std::cout << "*   --socket/-sc  <socket> parse code from socket                 *\n";
    std::cout << "*******************************************************************\n";
}

void Program::startInterpreter()
{
    std::cout << "Hello from interpreter!" << std::endl;
}