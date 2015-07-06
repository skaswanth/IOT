<?php

/*
 *  @user=MiTh
 */
require_once("Rest.inc.php");
require_once ("KLogger.php");

class API extends REST {

  public $jsonObj = array();

  //private $db = NULL;
  private $log;
  public function __construct() {
    parent::__construct();    // Init parent contructor
  // commntin here  $this->dbConnect();     // Initiate Database connection
  
  $this->fireMe();
  }

  public function processApi() {

    $func = "defualt_func";
    $this->log = new KLogger("log.txt", KLogger::DEBUG);
    if ($this->get_request_method() == "GET") {
      $func = strtolower(filter_input(INPUT_GET, 'func', FILTER_SANITIZE_SPECIAL_CHARS, array('flags' => FILTER_FLAG_STRIP_LOW)));
    } else {
      $func = $this->_request['func'];
    }

    if ((int) method_exists($this, $func) > 0) {
      try {
        $this->$func();
      } catch (Exception $e) {
        echo 'Caught exception: ', $e->getMessage(), "\n";
      }
    } else {
      #$this->response('function not exist', 404);
    }
  }

  private function json($data) {
    if (is_array($data)) {
      return json_encode($data);
    }
  }

  public function checkAndAssign(&$item1, $key, $defualt) {
    if (isset($this->_request[$key]) ? !empty($this->_request[$key]) : false)
      $item1 = $this->_request[$key];
    else {
      $this->valid = false;
      $this->validationMsg[] = "$key not exists";
      $item1 = $defualt;
    }
  }
  
   private function fireMe() {
   
   // $com = shell_exec("sudo ./off");
   // echo $com;
    $this->valid = TRUE;
    $this->validationMsg = array("success");
	
	 //Get name  and password 
	if (isset($this->_request['com']))
		$com = $this->_request['com'];
  
  //Execute shell command and get the response. 
  // 
$value=$_GET["device"];
//echo $value;
$result = shell_exec("sudo ./gettingstarted $value");
//echo $result;
//echo $result[13];
   
 //if ($result[13]=='0')
     // echo '<table style="font-size:50px; font-family:Arial; color:red;"><tr><td>Switch OFF</td></tr></table>';  
        
 //if ($result[13]=='1')
     //echo '<table style="font-size:50px; font-family:Arial; color:blue;"><tr><td>Switch ON</td></tr></table>'; 
    error_log("toggleSwitch");
	error_log($com);
	
    $success = array('status' => $result[13], 'param' => 1, "msg" => "success");
	$this->response($this->json($success), 200);
	return true;
   
   }
private function toggleSwitch() {

    $this->valid = TRUE;
    $this->validationMsg = array("success");
	
	 //Get name  and password 
	if (isset($this->_request['com']))
		$com = $this->_request['com'];
  
  //Execute shell command and get the response. 
  // 
$result = shell_exec($com);
  
    error_log("toggleSwitch");
	error_log($com);
	
 //  $success = array('status' => "success", 'param' => 1, "msg" => "success");
	//$this->response($this->json($success), 200);
	return true;
  }

  }

$api = new API;
$api->processApi();
