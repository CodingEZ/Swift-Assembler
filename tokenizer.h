class Tokenizer {

  var allTokens = [[Token]]()
  var allLabels = [String]()

  let letterSet: Set<String> = ["a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
              "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x",
              "y", "z", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L",
              "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"]

  let digitSet: Set<String> = ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"]

  let instructions: Dictionary<String, Int> = ["halt" : 0, "clrr" : 1,
          "clrx" : 2, "clrm" : 3, "clrb" : 4, "movir" : 5, "movrr" : 6, "movrm" : 7,
          "movmr" : 8, "movxr" : 9, "movar" : 10, "movb" : 11, "addir" : 12, "addrr" : 13,
          "addmr" : 14, "addxr" : 15, "subir" : 16, "subrr" : 17, "submr" : 18, "subxr" : 19,
          "mulir" : 20, "mulrr" : 21, "mulmr" : 22, "mulxr" : 23, "divir" : 24, "divrr" : 25,
          "divmr" : 26, "divxr" : 27, "jmp" : 28, "sojz" : 29, "sojnz" : 30, "aojz" : 31,
          "aojnz" : 32, "cmpir" : 33, "cmprr" : 34, "cmpmr" : 35, "jmpn" : 36, "jmpz" : 37,
          "jmpp" : 38, "jsr" : 39, "ret" : 40, "push" : 41, "pop" : 42, "stackc" : 43,
          "outci" : 44, "outcr" : 45, "outcx" : 46, "outcb" : 47, "readi" : 48, "printi" : 49,
          "readc" : 50, "readln" : 51, "brk" : 52, "movrx" : 53, "movxx" : 54, "outs" : 55,
          "nop" : 56, "jmpne" : 57 ]

  let expectedParameters: [String : [String]] = [ ".string" : ["string"], ".tuple" : ["tuple"],
              ".integer" : ["#"], ".end" : [], ".allocate" : ["#"], ".start": ["label"],
              "halt" : [], "clrr" : ["r1"], "clrx" : ["r1"], "clrm" : ["label"],
              "clrb" : ["r1", "r1"], "movir" : ["#", "r1"], "movrr" : ["r1", "r1"], "movrm" : ["r1", "label"],
              "movmr" : ["label", "r1"], "movxr" : ["r1", "r1"], "movar" : ["label", "r1"], "movb" : ["r1", "r1", "r1"],
              "addir" : ["#", "r1"], "addrr" : ["r1", "r1"], "addmr" : ["label", "r1"], "addxr" : ["r1", "r1"],
              "subir" : ["#", "r1"], "subrr" : ["r1", "r1"], "submr" : ["label", "r1"], "subxr" : ["r1", "r1"],
              "mulir" : ["#", "r1"], "mulrr" : ["r1", "r1"], "mulmr" : ["label", "r1"], "mulxr" : ["r1", "r1"],
              "divir" : ["#", "r1"], "divrr" : ["r1", "r1"], "divmr" : ["label", "r1"], "divxr" : ["r1", "r1"],
              "jmp" : ["label"], "sojz" : ["r1", "label"], "sojnz" : ["r1", "label"], "aojz" : ["r1", "label"],
              "aojnz" : ["r1", "label"], "cmpir" : ["#", "r1"], "cmprr" : ["r1", "r1"], "cmpmr" : ["label", "r1"],
              "jmpn" : ["label"], "jmpz" : ["label"], "jmpp" : ["label"], "jsr" : ["label"],
              "ret" : [], "push" : ["r1"], "pop" : ["r1"], "stackc" : ["r1"],
              "outci" : ["#"], "outcr" : ["r1"], "outcx" : ["r1"], "outcb" : ["r1", "r1"],
              "readi" : ["r1", "r1"], "printi" : ["r1"], "readc" : ["r1"], "readln" : ["label", "r1"],
              "brk" : [], "movrx" : ["r1", "r1"], "movxx" : ["r1", "r1"], "outs" : ["label"],
              "nop" : [], "jmpne" : ["label"] ]

  func offsetter(string: String, offset: Int) -> String {
    let end = string.index(string.startIndex, offsetBy: offset)
    return String(string[end])
  }

  func charRemover(_ input: String, end: Bool, start: Bool) -> String {
    var returnString = ""
    var chars = Array( input.characters )
    if end { chars.remove(at: chars.count - 1) }
    if start { chars.remove(at: 0) }
    for char in chars {
      returnString += String(char)
    }
    return returnString
  }

  func letterNumberString(_ chunk: String) -> Bool {
    let newChunk = charRemover(chunk, end: true, start: false)
    for char in newChunk.characters {
      if !letterSet.contains(String(char)) && !digitSet.contains(String(char)) {
        return false
      }
    }
    return true
  }

  func getPossibleLabels(_ chunks: [String]) {
    if chunks.count > 0 {
      let char = chunks[0].characters
      let lastChar = String( Array(char)[char.count - 1] )
      if lastChar == ":" {
        let newLabel = charRemover(chunks[0], end: true, start: false)
        allLabels.append(newLabel)
      }
    }
  }

  func makeLineOfTokens(_ chunks: [String]) {

    var analyzingChunks = chunks
    var tokenLine = [Token]()

    if analyzingChunks.count > 0 {
      let firstChunk = analyzingChunks[0]
      let char = firstChunk.characters
      let lastChar = offsetter(string: firstChunk, offset: char.count - 1)
      if lastChar == ":" {
        if letterSet.contains( String( firstChunk[firstChunk.startIndex] ) ) {
          if expectedParameters.keys.contains(firstChunk) {
            tokenLine.append( Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(firstChunk) - Expected Label Definition: cannot be name of directive or instruction.") )
          } else {
            if letterNumberString(firstChunk) {
              let newLabel = charRemover(chunks[0], end: true, start: false)
              tokenLine.append( Token(type: .LabelDefinition, intValue: nil, stringValue: newLabel, tupleValue: nil, description: "\(analyzingChunks[0])") )
            } else {
              tokenLine.append( Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(firstChunk) - Expected Label Definition: must contain only letters and numbers.") )
            }
          }
        } else {
          tokenLine.append( Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(firstChunk) - Expected Label Definition: must start with a letter.") )
        }
        analyzingChunks.remove(at: 0)
      }
    }

    if analyzingChunks.count > 0 {
      let chunk = analyzingChunks[0]
      if let x = expectedParameters[chunk] {

        if (chunk[chunk.startIndex] == ".") && (tokenLine.count == 0) && (chunk != ".end") && (chunk != ".start") {
          tokenLine.append( Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(chunk) - Expected label definition or instruction: first element cannot be directive that is not .end.") )
        }

        let correctCount = x.count + 1
        //print(correctCount, analyzingChunks.count)
        if analyzingChunks.count != correctCount {

          for _ in analyzingChunks {
            tokenLine.append( Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "Incorrect number of parameters. \(chunk) should have \(x.count)") )
          }

        } else {

          tokenLine.append( makeDirectiveOrInstruction(chunk) )
          var lineCounter = 1
          for eachChunk in x {
            switch eachChunk {
              case "r1":
                tokenLine.append( makeRegister(analyzingChunks[lineCounter]) )
              case "label":
                tokenLine.append( makeLabel(analyzingChunks[lineCounter]) )
              case "#":
                tokenLine.append( makeInteger(analyzingChunks[lineCounter]) )
              case "string":
                tokenLine.append( makeString(analyzingChunks[lineCounter]) )
              case "tuple":
                tokenLine.append( makeTuple(analyzingChunks[lineCounter]) )
              default:
                print("Error")
            }
            lineCounter += 1
          }

        }

      } else {
        for i in 0..<analyzingChunks.count {
          if i == 0 {
            tokenLine.append( Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(chunk) - Invalid instruction or directive.") )
          } else {
            tokenLine.append( Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(chunk) - See first error case in line.") )
          }
        }
      }

    }

    allTokens.append(tokenLine)

  }

  func makeRegister(_ chunk: String) -> Token {

    if chunk.characters.count == 2 {

      if chunk[chunk.startIndex] == "r" {
        let secondChar = offsetter(string: chunk, offset: 1)
        guard let integer = Int( secondChar ) else {
          return Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(chunk) - Expected Register: should have integer.")
        }
        return Token(type: .Register, intValue: integer, stringValue: nil, tupleValue: nil, description: "\(chunk)")
      } else {
        return Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(chunk) - Expected Register: should start with r.")
      }

    } else {
      return Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(chunk) - Expected Register: should have length 2.")
    }

  }

  func makeInteger(_ chunk: String) -> Token {
    if chunk[chunk.startIndex] == "#" {
      let string = charRemover(chunk, end: false, start: true)

      guard let integer = Int(string) else {
        return Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(chunk) - Expected Integer: not an integer.")
      }
      return Token(type: .ImmediateInteger, intValue: integer, stringValue: nil, tupleValue: nil, description: "\(chunk)")
    } else {
      return Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(chunk) - Expected Integer: must have hashtag.")
    }
  }

  func makeString(_ chunk: String) -> Token {
    let lastChar = offsetter(string: chunk, offset: chunk.characters.count - 1)
    if ( chunk[chunk.startIndex] == "\"" ) && ( lastChar == "\"" ) {
      let string = charRemover(chunk, end: true, start: true)
      return Token(type: .ImmediateString, intValue: nil, stringValue: string, tupleValue: nil, description: "\(chunk)")
    } else {
      return Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(chunk) - Expected String: missing quotation mark.")
    }
  }

  func makeTuple(_ chunk: String) -> Token {
    let lastChar = offsetter(string: chunk, offset: chunk.characters.count - 1)
    if ( chunk[chunk.startIndex] == "/" ) && ( lastChar == "/" ) {
      let string = charRemover(chunk, end: true, start: true)

      if string.characters.count != 5 {
        return Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(chunk) - Expected Tuple: nust have length 5.")
      }

      guard let int1 = Int( String( string[string.startIndex] ) ) else {
        return Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(chunk) - Expected Tuple: first element must be integer.")
      }

      let secondChar = offsetter(string: string, offset: 1)

      guard let int2 = Int( secondChar ) else {
        return Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(chunk) - Expected Tuple: second element must be integer.")
      }

      let thirdChar = offsetter(string: string, offset: 2)

      guard let int3 = Int( thirdChar ) else {
        return Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(chunk) - Expected Tuple: third element must be integer.")
      }

      let fourthChar = offsetter(string: string, offset: 3)

      guard let int4 = Int( fourthChar ) else {
        return Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(chunk) - Expected Tuple: fourth element must be integer.")
      }

      let fifthChar = offsetter(string: string, offset: 4)

      if (fifthChar != "r") && (fifthChar != "R") {
        return Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(chunk) - Expected Tuple: fifth element must be \"r\" or \"R\".")
      } else {
        return Token(type: .ImmediateTuple, intValue: nil, stringValue: nil, tupleValue: (int1, int2, int3, int4, fifthChar), description: "\(chunk)")
      }

    } else {
      return Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(chunk) - Expected Tuple: missing backslash.")
    }

  }

  func makeLabel(_ chunk: String) -> Token {
    if allLabels.contains(chunk) {
      return Token(type: .Label, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(chunk)")
    } else {
      return Token(type: .BadToken, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(chunk) - Expected Label: label not yet defined.")
    }
  }

  func makeDirectiveOrInstruction(_ chunk: String) -> Token {
    if chunk[chunk.startIndex] == "." {
      return Token(type: .Directive, intValue: nil, stringValue: nil, tupleValue: nil, description: "\(chunk)")
    } else {
      return Token(type: .Instruction, intValue: instructions[chunk]!, stringValue: nil, tupleValue: nil, description: "\(chunk)")
    }
  }

  func clearTokenizerVariables() {
    allTokens = []
    allLabels = []
  }

}
