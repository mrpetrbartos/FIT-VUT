from selenium import webdriver
from selenium.webdriver.common.desired_capabilities import DesiredCapabilities


def before_feature(context, feature):
    context.driver = webdriver.Remote(
        command_executor="http://localhost:4444/wd/hub",
        desired_capabilities=DesiredCapabilities.CHROME,
    )
    context.driver.implicitly_wait(15)
    context.base_url = "http://mys01.fit.vutbr.cz:8087/"


def after_feature(context, feature):
    context.driver.quit()
