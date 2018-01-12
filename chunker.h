class Chunker {

  var codeLines = [String]()
  var allChunks = [[String]]()

  func readCodeFile(_ path: String) -> [String] {
    let text: String
    do {
      text = try String(contentsOfFile: path, encoding: String.Encoding.utf8)
    } catch {
      codeLines = [String]()
      return codeLines
    }
    codeLines = splitStringIntoLines(text)
    return codeLines
  }

  func clearCodeLines() {
    codeLines = []
  }

  func clearChunks() {
    allChunks = []
  }

  func splitStringIntoLines(_ expression: String) -> [String] {
    return expression.characters.split{$0 == "\n"}.map{ String($0) }
  }

  func searchLine(_ line: String) {
    let lineChars = Array(line.characters)
    let end = lineChars.count
    var charPointer = 0
    var currentChunk = ""
    var chunks = [String]()

    while charPointer != end {
      //print(currentChunk)
      switch lineChars[charPointer] {

        case " ", "\t":
          charPointer += 1

        case "\"":
          repeat {
            currentChunk += String(lineChars[charPointer])
            charPointer += 1
          } while (charPointer != end) && (lineChars[charPointer] != "\"")

          currentChunk += "\""

          if charPointer != end {
            //currentChunk += String(lineChars[charPointer])
            charPointer += 1
          }



          chunks.append(currentChunk)
          currentChunk = ""

          case "/":
            repeat {
              currentChunk += String(lineChars[charPointer])
              charPointer += 1
            } while (charPointer != end) && (lineChars[charPointer] != "/")

            if charPointer != end {
              currentChunk += String(lineChars[charPointer])
              charPointer += 1
            }

            chunks.append(currentChunk)
            currentChunk = ""

        case ";":
          repeat {
            //currentChunk += String(lineChars[charPointer])
            charPointer += 1
          } while charPointer != end

          //chunks.append(currentChunk)
          currentChunk = ""

        default:
          while (charPointer != end) && (lineChars[charPointer] != " ") && (lineChars[charPointer] != "\t") {
            currentChunk += String(lineChars[charPointer])
            charPointer += 1
          }

          currentChunk = currentChunk.lowercased()
          chunks.append(currentChunk)
          currentChunk = ""
      }
    }

    allChunks.append(chunks)
  }

}
