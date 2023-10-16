<?php
/**
 * IPP Project 1 - Parser
 * 
 * Parser for IPPcode23, unstructured intermediate language used at
 * FIT BUT for the Principles of Programming Languages course.
 * 
 * PHP version 8.1
 *
 * Author: Petr Bartoš (xbarto0g)
 */

ini_set('display_errors', 'stderr');

$headSeen = 0;
$insCnt = 0;

// Each instruction is represented as named key, which stores an array of its parameters
$instructions = array(
    "MOVE" => array("v", "s"),
    "CREATEFRAME" => array(),
    "PUSHFRAME" => array(),
    "POPFRAME" => array(),
    "DEFVAR" => array("v"),
    "CALL" => array("l"),
    "RETURN" => array(),

    "PUSHS" => array("s"),
    "POPS" => array("v"),

    "ADD" => array("v", "s", "s"),
    "SUB" => array("v", "s", "s"),
    "MUL" => array("v", "s", "s"),
    "IDIV" => array("v", "s", "s"),
    "LT" => array("v", "s", "s"),
    "GT" => array("v", "s", "s"),
    "EQ" => array("v", "s", "s"),
    "AND" => array("v", "s", "s"),
    "OR" => array("v", "s", "s"),
    "NOT" => array("v", "s"),
    "INT2CHAR" => array("v", "s"),
    "STRI2INT" => array("v", "s", "s"),

    "READ" => array("v", "t"),
    "WRITE" => array("s"),

    "CONCAT" => array("v", "s", "s"),
    "STRLEN" => array("v", "s"),
    "GETCHAR" => array("v", "s", "s"),
    "SETCHAR" => array("v", "s", "s"),

    "TYPE" => array("v", "s"),

    "LABEL" => array("l"),
    "JUMP" => array("l"),
    "JUMPIFEQ" => array("l", "s", "s"),
    "JUMPIFNEQ" => array("l", "s", "s"),
    "EXIT" => array("s"),

    "DPRINT" => array("s"),
    "BREAK" => array()
);

if ($argc != 1)
    checkParams($argv, $argc);

// Create root XML element
$root = new SimpleXMLElement('<?xml version="1.0" encoding="UTF-8"?><program></program>');
$root->addAttribute("language", "IPPcode23");

// Verify that the file starts with .ippcode23 header
while($f = fgets(STDIN))
{
    // Remove leading and trailing whitespaces, don't parse empty lines
    $f = trim($f);
    if ($f === "" || $f[0] === "#")
        continue;
    if (!preg_match("/^\.(ippcode23)(\s*\#.*)?$/i", $f))
    {
        fwrite(STDERR, "Source code has to start with a language identificator.\n");
        exit(21);  
    }
    break;
}

while($f = fgets(STDIN))
{
    $f = trim($f);
    if ($f === "" || $f[0] === "#")
        continue;
    parseLine($f);
}

// Correctly indent output
$dom = dom_import_simplexml($root)->ownerDocument;
$dom->formatOutput = true;
echo $dom->saveXML();

// Processes a nonempty line and validates lexical and syntactic correctness  
function parseLine($line)
{
    global $instructions, $headSeen, $insCnt, $root;
    $argCnt = 0;
    // Replace every sequence of white characters with a single space
    $line = preg_replace("/\s\s+/", " ", $line);
    // Discard everything after and including '#' character
    if (str_contains($line, '#'))
        $line = trim(substr($line, 0, strpos($line, "#")));
    // Split the line into operads to verify them individually
    $operands = explode(" ", $line);
    $opcode = strtoupper($operands[0]);

    if (!isset($instructions[$opcode]))
    {
        fwrite(STDERR, "$operands[0] is not valid opcode.\n");
        exit(22); 
    }

    if (count(array_slice($operands, 1)) != count($instructions[$opcode]))
    {
        fwrite(STDERR, "Incorrect number of parameters provided.\n");
        exit(23); 
    }

    $ins = $root->addChild("instruction", "");
    $ins->addAttribute("order", strval(++$insCnt));
    $ins->addAttribute("opcode", $opcode);

    foreach (array_values($instructions[$opcode]) as $i => $optype)
    {
        // Skip the first element, which is the opcode
        $operand = $operands[$i + 1];
        // Fix incorrect printing of '&' character
        $argContent = preg_replace("/(?:bool|int|string|nil)@/", "", str_replace("&", "&amp;", $operand));
        $arg = $ins->addChild("arg" . strval(++$argCnt), $argContent);
        
        switch ($optype)
        {
            case "l":
                isLabel($operand);
                $arg->addAttribute("type", "label");
                break;
            case "v":
                isVar($operand);
                $arg->addAttribute("type", "var");
                break;
            case "s":
                $arg->addAttribute("type", isSymb($operand));
                break;
            case "t":
                isType($operand);
                $arg->addAttribute("type", "type");
                break;
        }
    }
}

function isLabel($operand)
{
    if (!preg_match("/^[A-Za-z_\-$&%*!?][A-Za-z0-9_\-$&%*!?]*$/", $operand))
        wrongParam("label");
}

function isVar($operand)
{
    if (!preg_match("/^(?:LF|TF|GF)@[A-Za-z_\-$&%*!?][A-Za-z0-9_\-$&%*!?]*$/", $operand))
        wrongParam("variable");
}

function isSymb($operand)
{
    if ($operand == "nil@nil")
        return "nil";

    if ($operand == "bool@true" || $operand == "bool@false")
        return "bool";

    $intReg = "/^(int)@[+-]?(";
    // decimal
    $intReg .= "(?:[1-9][0-9]*(_[0-9]+)*|0)";
    // hexadecimal
    $intReg .= "|(?:0[xX][0-9a-fA-F]+(_[0-9a-fA-F]+)*)";
    // octal
    $intReg .= "|(?:0[oO]?[0-7]+(_[0-7]+)*))$/";
    if (preg_match($intReg, $operand))
        return "int";

    if (preg_match("/^(?:LF|TF|GF)@[A-Za-z_\-$&%*!?][A-Za-z0-9_\-$&%*!?]*$/", $operand))
        return "var";

    if (preg_match("/^(string)@(?:[^\\\\]|(?:\\\\\d{3}))*$/", $operand))
        return "string";

    wrongParam("symbol");
}

function isType($operand)
{
    $types = array("int", "bool", "string");
    if (!in_array($operand, $types))
        wrongParam("type");
}

function wrongParam($type)
{
    fwrite(STDERR, "Wrong $type parameter provided.\n");
    exit(23);   
}

function checkParams($argv, $argc)
{
    if ($argv[1] != "--help" || $argc != 2)
    {
        fwrite(STDERR, "Only 'help' parameter can be used.\n");
        exit(10);
    } else {
        printHelp();
    }
}

function printHelp()
{
    echo("IPPcode23 parser\n");
    echo("Reads IPPcode23 from STDIN, validates it, and prints its XML representation\n");
    echo("Usage: php parse.php [--help]\n");
    exit(0);
}
