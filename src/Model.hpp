#pragma once
#include <halp/controls.hpp>
#include <halp/controls.enums.hpp>
#include <halp/messages.hpp>
#include <halp/meta.hpp>

#include <algorithm>
#include <cctype>
#include <string>

// MyDataProcessor is a "data" / message object: it processes strings (and other
// non-audio data), not sound or pixels. The same object compiles, without rewrite,
// to the back-ends with a real object model (see avnd_make_object):
//   - Max/MSP and Pure Data externals
//   - a TouchDesigner CHOP (message)
//   - a Godot GDExtension node
//   - a Python extension module
//   - an ossia score process
// It is intentionally NOT an audio / VST3 / CLAP plug-in.
//
// See examples/Complete/CompleteMessageExample.hpp in Avendish for the full set of
// string / message / callback features.
class MyDataProcessor
{
public:
  halp_meta(name, "My Data Processor")
  halp_meta(c_name, "my_data_processor")
  halp_meta(category, "Data")
  halp_meta(author, "Avendish")
  halp_meta(description, "Transform an input string (case / reverse, optional prefix)")

  // CHANGE THIS !! (uuidgen)
  halp_meta(uuid, "6b1d9f2c-0e84-4a7d-9c3b-2f5a8d41e770")

  enum class op
  {
    uppercase,
    lowercase,
    reverse
  };

  struct ins
  {
    // The string to transform; in Max/Pd a symbol on the inlet. Recomputes on input.
    struct : halp::val_port<"Input", std::string>
    {
      void update(MyDataProcessor& self) { self(); }
    } text;

    // Which transformation to apply.
    halp::enum_t<op, "Operation"> operation;
  } inputs;

  struct
  {
    // The transformed string, sent on the outlet.
    halp::val_port<"Output", std::string> result;
  } outputs;

  // Recompute the output from the current input. Defined inline (Avendish back-ends
  // compile this header directly and several do not link the object library).
  void operator()();

  // Message handler: "set_prefix <symbol>" prepends a prefix to every output.
  void set_prefix(std::string p) { prefix = std::move(p); }
  halp_start_messages(MyDataProcessor)
    halp_mem_fun(set_prefix)
  halp_end_messages

private:
  std::string prefix;
};

inline void MyDataProcessor::operator()()
{
  std::string out = inputs.text.value;

  switch(inputs.operation.value)
  {
    case op::uppercase:
      std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
        return std::toupper(c);
      });
      break;
    case op::lowercase:
      std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
        return std::tolower(c);
      });
      break;
    case op::reverse:
      std::reverse(out.begin(), out.end());
      break;
  }

  outputs.result.value = prefix + out;
}
