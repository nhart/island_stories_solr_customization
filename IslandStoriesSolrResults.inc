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
    
    foreach ($docs as $num => $doc) {       
        $i++;
    	$PID = $doc['PID'];
    	$title = $doc['title'][0];
        $city = $doc['mods.city'][0];
        $collection = $doc['rels.isMemberOfCollection'][0];
        
        if($collection == "info:fedora/istories:communities") {
            $link = "islandora/solr/search/mods.city:$city";            
            
        }
        else {
            $link = $fedora_url;
        }
        
        //l('<img src="path/to/image.jpg"/>', $filepath, $options = array('html' => TRUE));
    	$cols[] = l("<img src=\"$fedora_url/get/$PID/TN\" alt=\"$title\"/><br />".$title,$link,$options = array('html' => TRUE));
        if($i == $numCols) {
          $rows[] = $cols;
          $i=0;
          $cols = "";
        }
        
    }
    if($i < $numCols) {
          $rows[] = $cols;
          $i=0;
          $cols = "";
    }
    
    $output .= theme_table(null,$rows);
    
    
    
    
    
    //$uri = request_uri();
    //$base_path = base_path();
    //$redirect = preg_replace($base_path, '', $uri);
    //$redirect = preg_replace('/\/\//', '/', $redirect);

    //$_SESSION['search_url'] = $redirect;
    return '<div class ="islandora-solr-search">' . $output . '</div>';
  }

}