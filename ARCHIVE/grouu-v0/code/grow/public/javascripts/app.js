'use strict';


// Declare app level module which depends on filters, and services
var app = angular.module('myApp', ['myApp.filters', 'myApp.directives'])
.
  config(['$routeProvider', '$locationProvider', function($routeProvider, $locationProvider) {
    $routeProvider.
      when('/', {
        templateUrl: 'partials/index',
        controller: AppCtrl
      }).
      when('/environment', {
        templateUrl: 'partials/environment',
        controller: EnvironmentCtrl
      }).
      when('/trial', {
        templateUrl: 'partials/trial',
        controller: TrialCtrl
      }).
      when('/trial/:trialId', {
        templateUrl: 'partials/trialONE',
        controller: TrialONECtrl
      }).
      when('/probe/:probeId', {
        templateUrl: 'partials/probe',
        controller: ProbeCtrl
      }).      
       when('/watering', {
        templateUrl: 'partials/watering',
        controller: WateringCtrl
      }).
       when('/reports', {
        templateUrl: 'partials/reports',
        controller: ReportsCtrl
      }).
        when('/journal', {
        templateUrl: 'partials/journal',
        controller: JournalCtrl
      }).
       when('/dashboard', {
        templateUrl: 'partials/dashboard',
        controller:DashboardCtrl 
      });//.
      // otherwise({
      //   redirectTo: '/'
      // });
    $locationProvider.html5Mode(true);
  }]);
