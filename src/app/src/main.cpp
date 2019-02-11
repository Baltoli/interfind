#include <fmt/format.h>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/SourceMgr.h>

#include <nlohmann/json.hpp>

#include <filesystem>
#include <string>

using namespace llvm;
using json = nlohmann::json;
namespace fs = std::filesystem;

/**
 * The input bitcode file to process. Reads from a filename by default, but can
 * be passed '-' as a special case to read from stdin if that's the desired
 * behaviour (e.g. in a pipeline).
 */
cl::opt<std::string> InputFilename(
    cl::Positional, cl::desc("<input file>"), cl::Required);

/**
 * The path of a configuration file specifying how to examine the IR in
 * question. The config is free-form JSON and individual components of InterFind
 * will complain if they don't have the right information passed to them.
 */
cl::opt<std::string> ConfigPath(
    cl::Positional, cl::desc("<config file>"), cl::Required);

/**
 * Silence the analysis output from the tool - if it's silent then any results
 * won't be printed anywhere. This might be useful if you're running the tool
 * in the context of a build system.
 */
cl::opt<bool> Silent(
    "silent", cl::desc("Don't print analysis output anywhere"));
cl::alias SilentShort(
    "s", cl::desc("Alias for -silent"), cl::aliasopt(Silent));

/**
 * Just perform the analysis and output the results without actually changing
 * the bitcode - the tool will act as a pass-through compiler.
 */
cl::opt<bool> AnalysisOnly(
    "analysis-only", cl::desc("Don't modify bitcode - just perform analysis"));
cl::alias AnalysisOnlyShort(
    "A", cl::desc("Alias for -analysis-only"), cl::aliasopt(AnalysisOnly));

/**
 * Where to output the analysis results. The default is to a file with a name
 * derived from the input bitcode filename, but can be passed any file name or
 * '-' to output to standard error.
 */
cl::opt<std::string> AnalysisOutput(
    "analysis-output", cl::desc("Analysis output filename"), cl::init(""));
cl::alias AnalysisOutputShort(
    "a", cl::desc("Alias for -analysis-output"), cl::aliasopt(AnalysisOutput));

/**
 * Where to output the modified bitcode generated by the tool. If the value is
 * empty, then output to a filename derived from the input filename. Can be
 * passed '-' to output to standard out in textual format.
 */
cl::opt<std::string> Output(
    "output", cl::desc("Output bitcode filename"), cl::init(""));
cl::alias OutputShort(
    "o", cl::desc("Alias for -output"), cl::aliasopt(Output));

int main(int argc, char **argv) try
{
  cl::ParseCommandLineOptions(argc, argv);

  auto ctx = LLVMContext{};
  auto err = SMDiagnostic{};

  auto mod = parseIRFile(InputFilename, err, ctx, true, "");
  if(!mod) {
    err.print(argv[0], errs());
    return 1;
  }

  auto config_path = fs::path(ConfigPath.getValue());
  if(!fs::exists(config_path)) {
    errs() << fmt::format(
      "Config file does not exist: {}\n", config_path.string()
    );

    return 2;
  }

  auto buffer = MemoryBuffer::getFile(config_path.string());
  if(auto err_code = buffer.getError()) {
    errs() << fmt::format(
      "Error opening memory buffer from file: {}\nError: {}\n", 
      config_path.string(),
      err_code.message()
    );
  }

  auto json = json::parse(
    buffer.get()->getBufferStart(), 
    buffer.get()->getBufferEnd());
} catch(json::parse_error const& pe) {
  errs() << fmt::format("Error parsing JSON file: {}\n", pe.what());
  std::exit(3);
}
