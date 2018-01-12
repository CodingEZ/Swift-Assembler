enum TokenType: String {
  case Register = "Register"
  case LabelDefinition = "LabelDefinition"
  case Label = "Label"
  case ImmediateTuple = "ImmediateTuple"
  case ImmediateString = "ImmediateString"
  case ImmediateInteger = "ImmediateInteger"
  case Instruction = "Instruction"
  case Directive = "Directive"
  case BadToken = "BadToken"
}

struct Token: CustomStringConvertible {
  let type: TokenType
  let intValue: Int?
  let stringValue: String?
  let tupleValue: (Int, Int, Int, Int, String)?
  let description: String
}
