Feature: Navigation

    # 01
    Scenario: Page can be reached
        Given a web browser is open
        And the user has access to the internet
        When the user tries to access the site
        Then home page is displayed

    # 02
    Scenario: View products category
        Given a web browser is at the home page
        When the user clicks on a non-empty category in the navigation bar
        Then products from the selected category should be displayed

    # 03
    Scenario: View empty products category
        Given a web browser is at the home page
        When the user navigates to a category with no products
        Then no products message should be displayed

    # 04
    Scenario: View featured product
        Given a web browser is at the home page
        When the user clicks on a featured product
        Then the selected product should be displayed

    # 05
    Scenario: Search for a product
        Given a web browser is at the home page
        And search query has been typed into Search input
        When the user clicks on the search button
        Then all product with given string in their names should be displayed

    # 06
    Scenario: Change currency
        Given a web browser is at the home page
        When the user selects a currency using the top bar
        Then page should display the price in the selected currency

    # 07
    Scenario Outline: Order shown products
        Given a web browser is at the home page
        And the user searches for a product
        And multiple products are shown
        When the user selects <SortBy> sort option from dropdown
        Then the shown products should be sorted by <SortBy> parameter
        Examples:
            | SortBy |
            | Name   |
            | Price  |
            | Rating |
            | Model  |
