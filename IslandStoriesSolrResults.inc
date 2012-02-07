<?php

/*
 * contains methods to search solr and display results.  depends on Apache_Solr_Php client.
 */


/**
 * Extention of IslandoraSolrResults
 * This overrides the printResults function to provide customized display.
 */
module_load_include('inc', 'islandora_solr_search', 'IslandoraSolrResults');

class IslandStoriesSolrResults extends IslandoraSolrResults {

  static $facetSeparator = '~'; //used to separate facets in url
  static $slashReplacement = '~slsh~'; // a pattern used to replace / in url's the slash breaks drupal clean url's
  public static $SEARCH_CLASS_ADVANCED_SEARCH_NUMBER_FIELDS = 5;

  function IslandStoriesSolrResults() {
    module_load_include('php', 'islandora_solr_search', 'Solr/Service');
    
  }

  /**
   *
   * @param $results
   *   A solr resultset object.
   *
   * @return
   *   html output for the resultset. Note: we currently create this
   *   output manually, should refactor to use drupal forms api.
   */
  function printResults($results) {
    module_load_include('inc', 'fedora_repository', 'api/fedora_item');    
    
    global $base_url;
    
    $recordStart = (int) $results->response->start;
    if (empty($results)) {
      return "no results";
    }
    $rawResponse = $results->getRawResponse();
    $responseArray = json_decode($rawResponse, true);
    $docs = $responseArray['response']['docs'];
    
   
    $rows = array();
    $cols = array();
    $numCols = 3;
    $numFound = $responseArray['response']['numFound'];
    $paramRows = $responseArray['responseHeader']['params']['rows'];
    $i = 0;
    $fedora_url = variable_get('fedora_base_url','http://localhost:8080/fedora');
    
    //loop through all the indexed objects
    foreach ($docs as $doc) {       
        $i++;
    	$PID = $doc['PID'];
    	$title = $doc['title'][0];
        if(strlen($title) > 30) {
            $title = substr($title, 0, 30);        
            $title .= "...";
        }
        $city = $doc['mods.city'][0];
        //take out any reference to island in the city eg: lennox island
        $city = str_replace("Island", "", $city);
        $collection = $doc['rels.isMemberOfCollection'][0];
        //get the content model name
        $model = str_replace("info:fedora/", "", $doc['rels.hasModel'][0]);
        
        if($collection == "info:fedora/istories:communities") {
            $link = "communities/$city";
        }
        else {
            $link = $base_url ."/islandoraviewer/$PID/$model";
        }
        //fill the column with a TN, title and link them to the $link var
        $cols[] = l("<img src=\"$fedora_url/get/$PID/TN\" alt=\"$title\"/><br />".$title,$link,$options = array('html' => TRUE));
        //$cols[] = "test";
        if($i == $numCols) {
          $rows[] = $cols;
          $i=0;
          $cols = "";
        }        
    }
    
    //dump remaining results to a new row 
    //if they didn't complete a full row
    if($i < $numCols && is_array($cols)) {
        
        //fill the remainder of the row cells with blank
        for($i; $i <= $numCols; $i++) {
            $cols[] = "";
        }
        
        $rows[] = $cols;
        $i=0;
        $cols = "";
    }
    
   
    
    //theme the table with drupal function
    $output .= theme_table(null,$rows);
    
    
    
    
    
    //$uri = request_uri();
    //$base_path = base_path();
    //$redirect = preg_replace($base_path, '', $uri);
    //$redirect = preg_replace('/\/\//', '/', $redirect);

    //$_SESSION['search_url'] = $redirect;
    return '<div class ="islandora-solr-search">' . $output . '</div>';
  }

}