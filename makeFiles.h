import Foundation

class MakeFiles {

  var allBinary = [Int]()
  var labelTable = [String : Int]()
  var numberBadTokens = 0
  var errorList = [String]()

  enum Instruction {
    case halt, clrr, clrx, clrm, clrb, movir, movrr, movrm, movmr, movxr
    case movar, movb, addir, addrr, addmr, addxr, subir, subrr, submr, subxr
    case mulir, mulrr, mulmr, mulxr, divir, divrr, divmr, divxr, jmp, sojz
    case sojnz, aojz, aojnz, cmpir, cmprr, cmpmr, jmpn, jmpz, jmpp, jsr
    case ret, push, pop, stackc, outci, outcr, outcx, outcb, readi, printi
    case readc, readln, brk, movrx, movxx, outs, nop, jmpne
  }

  func characterToUnicodeValue(_ c: Character) -> Int {
      let s = String(c)
      return Int(s.unicodeScalars[s.unicodeScalars.startIndex].value)
  }

  func unicodeValueToCharacter(_ n: Int) -> Character {
    return Character(UnicodeScalar(n)!)
  }

  func binaryMakerPass1(_ tokens: [[Token]]) {
    var counter = -1
    for line in tokens {
      for token in line {
        switch token.type {
          case .Instruction, .Label, .ImmediateInteger, .Register: counter += 1
          case .LabelDefinition: labelTable[token.stringValue!] = counter
          case .ImmediateString:
            counter += 1 + token.stringValue!.characters.count
          case .ImmediateTuple: counter += 5
          default: let _ = 0 // for .Directive
        }
      }
    }
  }

  func binaryMakerPass2(_ tokens: [[Token]]) {
    for line in tokens {
      for token in line {
        switch token.type {
          case .Instruction: allBinary.append( token.intValue! )
          case .Label: allBinary.append( labelTable[token.description]! )
          case .ImmediateInteger, .Register: allBinary.append(token.intValue!)
          case .ImmediateString: convertString(token.stringValue!)
          case .ImmediateTuple: convertTuple(token.tupleValue!)
          default: let _ = 0 // for .LabelDefinition and .Directive
        }
      }
    }
    allBinary.insert(allBinary.count - 1, at: 0)
  }

  func convertString(_ string: String) {
    let chars = string.characters
    allBinary.append( chars.count )
    for char in chars {
      allBinary.append( characterToUnicodeValue(char) )
    }
  }

  func convertTuple(_ tuple: (Int, Int, Int, Int, String) ) {
    allBinary.append( tuple.0 )
    allBinary.append( tuple.1 )
    allBinary.append( tuple.2 )
    allBinary.append( tuple.3 )
    allBinary.append( characterToUnicodeValue( Character(tuple.4) ) )
  }

  func writeTextFile(_ path: [String], data: String) -> String? {
    let url = NSURL.fileURL(withPathComponents: path)
    do {
      try data.write(to: url!, atomically: true, encoding: String.Encoding.utf8)
    } catch let error as NSError {
      return "Failed writing to URL: \(url), Error: " + error.localizedDescription
    }
    return nil
  }

  func makeBinaryFile(_ location: [String], _ file: String) {
    var dataString = ""
    for binary in allBinary {
      dataString += (String(binary) + "\n")
    }
    let _ = writeTextFile(location + ["\(file).bin"], data: dataString)
  }

  func makeSymbolTable(_ location: [String], _ file: String, _ tokens: [[Token]]) {

    var dataString = ""

    for line in tokens {
      for token in line {
        dataString += token.type.rawValue + ": "

        if token.type == .ImmediateString {
          dataString += token.stringValue!
        } else if token.type == .ImmediateInteger {
          dataString += String(token.intValue!)
        } else if token.type == .ImmediateTuple {
          dataString += String(token.intValue!)
        } else {
          dataString += token.description
        }

        dataString += "\n"

      }
    }

    let _ = writeTextFile(location + ["\(file).sym"], data: dataString)

  }

  func makeListingFile(_ location: [String], _ file: String, _ errorList: [String], _ tokens: [[Token]], _ code: [String]) {
    var dataString = ""
    var x = 0

    if errorList.count > 0 {
      for error in errorList {
        dataString += error + "\n"
      }
    } else {

      var memoryCounter = 0

      for line in tokens {

        var lineString = String(memoryCounter) + ": "

        if line.count > 0 {
          if line[0].description == ".start" {
            let _ = 0
          } else {
            for token in line {
              switch token.type {
                case .Instruction, .ImmediateInteger, .Register :
                  lineString += String(token.intValue!)
                  memoryCounter += 1
                  lineString += " "
                case .Label:
                  lineString += String(labelTable[token.description]!)
                  memoryCounter += 1
                  lineString += " "
                case .ImmediateString:
                  lineString += binaryString(token.stringValue!, memoryCounter)
                  memoryCounter += 1 + token.stringValue!.characters.count
                  lineString += " "
                case .ImmediateTuple:
                  lineString += binaryTuple(memoryCounter)
                  memoryCounter += 5
                  lineString += " "
                default: let _ = 0 // for .Directive and .LabelDefinition
              }
            }
          }
        }

        let extraSpaces = 30 - lineString.characters.count
        for _ in 0..<extraSpaces {
          lineString += " "
        }

        lineString += code[x] + "\n"
        x += 1
        dataString += lineString

      }

      dataString += "\n\nSymbol Table:\n"
      for (key, value) in labelTable {
        dataString += "\(key) \(value)\n"
      }

    }

    let _ = writeTextFile(location + ["\(file).lst"], data: dataString)

  }

  func clearFiles(_ location: [String], _ file: String) {
    let _ = writeTextFile(location + ["\(file).bin"], data: "")
    let _ = writeTextFile(location + ["\(file).sym"], data: "")
    let _ = writeTextFile(location + ["\(file).lst"], data: "")
  }

  func binaryString(_ string: String, _ index: Int) -> String {
    var newString = ""
    for x in 0..<string.characters.count {
      newString += String(allBinary[index + x + 2]) + " "   // +2 because of first two elements in allBinary
      if x == 3 {
        return newString
      }
    }
    return newString
  }

  func binaryTuple(_ index: Int) -> String {
    var newString = ""
    for x in 0..<5 {
      newString += String(allBinary[index + x + 2])   // +2 because of first two elements in allBinary
    }
    return newString
  }

  func countTokens(_ tokens: [[Token]]) {
    for line in tokens {
      for token in line {
        if token.type == .BadToken {
          numberBadTokens += 1
        }
      }
    }
  }

  func addToErrorList(_ string: String) {
    errorList.append(string)
  }

  func clearMakeFilesVariables() {
    allBinary = []
    labelTable = [:]
    numberBadTokens = 0
    errorList = []
  }

}
