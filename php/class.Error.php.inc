<?php
namespace dw;

/*
	Class to hold error messages of all kind.
*/
class Error extends Core{
	public $errorType; //W = warning, E = Error
	public $header; //The header of the error message
	public $message; //The error message
	/*
		This is basically just to hold an error message as an object. Creating an error also inforces you to
		define its type, header and message
	*/
	public function __construct($errorType, $header, $message) {
		$this->errorType = $errorType;
		$this->header = $header;
		$this->message = $message;
	}
}