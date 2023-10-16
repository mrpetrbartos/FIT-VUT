Feature: Administration

    # 19
    Scenario: Administration panel can be reached
        Given a web browser is at the home page
        And the user appends "/administration" to the url
        When the user tries to access the admin site
        Then administration page should be displayed

    # 20
    Scenario: Admin can log in
        Given a web browser is at the admin login page
        And the administrator isn't logged in
        And the user fills in his credentials
        When the user clicks on Login button
        Then the user should be logged in to admin dashboard

    # 23
    Scenario: Create new category
        Given administrator is logged in
        And the user navigates to Categories page
        And the user clicks on Add New button
        And the user fills in all the required details
        When the user clicks on modification Save button
        Then success modification notification should be shown

    # 25
    Scenario: Update product details
        Given administrator is logged in
        And the user navigates to details of a product
        And the user modifies saved values
        When the user clicks on modification Save button
        Then success modification notification should be shown

