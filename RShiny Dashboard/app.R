# Import packages
library(tidyverse)
library(dplyr)
library(readr)
library(sf)
library(ggplot2)
library(leaflet)
library(scales)
library(stringr)

library(shiny)
library(bslib)

# Read in data
offence_rate_data <- read.csv("Data/project_data.csv", header=T)
lgas <- sf::st_read("Data/LGA_POLYGON.shp")

# Join offence rate data with LGA polygons
offence_rate_polygons <- left_join(lgas, offence_rate_data, by = c("LGA_NAME" = "LGA"))
offence_rate_polygons <- offence_rate_polygons[!is.na(offence_rate_polygons$Year), ]

# Calculate total rates for the whole state
whole_state_data <- aggregate(Rate ~ Year + Offence + Type, data = offence_rate_data, FUN = sum)

whole_state_type_total_data <- aggregate(Rate ~ Year + Type, data = whole_state_data, FUN = sum)

# Calculate increase in offences from 2016 to 2025
offence_rate_data_2016 <- subset(offence_rate_data, offence_rate_data$Year==2016)
offence_rate_data_2025 <- subset(offence_rate_data, offence_rate_data$Year==2025)

increase_data <- offence_rate_data_2025 %>%
  left_join(offence_rate_data_2016, by = c("LGA", "Offence", "Type"), suffix = c(".2025", ".2016")) %>%
  mutate(Rate = Rate.2025 - Rate.2016)

increase_data <- subset(increase_data, select = -c(Year.2016, Year.2025, Rate.2016, Rate.2025))

# Join increase data with polygons
increase_polygons <- left_join(lgas, increase_data, by = c("LGA_NAME" = "LGA"))

# Calculate total rates of FV crime and Non-FV crime
type_total_data <- aggregate(Rate ~ Year + LGA + Type, data = offence_rate_data, FUN = sum)

# Join type total data with polygons
type_total_polygons <- left_join(lgas, type_total_data, by = c("LGA_NAME" = "LGA"))
# Remove rows with NA in year column
type_total_polygons <- type_total_polygons[!is.na(type_total_polygons$Year), ]

# Calculate increase in total offences from 2016 to 2025
type_total_data_2016 <- subset(type_total_data, type_total_data$Year==2016)
type_total_data_2025 <- subset(type_total_data, type_total_data$Year==2025)


type_total_increase_data <- type_total_data_2025 %>%
  left_join(type_total_data_2016, by = c("LGA", "Type"), suffix = c(".2025", ".2016")) %>%
  mutate(Rate = Rate.2025 - Rate.2016)

type_total_increase_data <- subset(type_total_increase_data, select = -c(Year.2016, Year.2025, Rate.2016, Rate.2025))

# Join with polygons
type_total_increase_polygons <- left_join(lgas, type_total_increase_data, by = c("LGA_NAME" = "LGA"))


# Define UI ----
ui <- page_sidebar(
  title = "Family Violence in Victoria",
  sidebar = sidebar(
    selectInput(
      "map_type",
      label = "Choose map type",
      choices = list("Offence rate", "Increase in offence rate from 2016 to 2025"),
      selected = "Offence rate"
    ),
    
    selectInput(
      "offence",
      label = "Select offence type",
      choices = list("Serious assault", "Common assault", "Stalking", "Harassment and private nuisance", "Threatening behaviour", "All offences"),
      selected = "All offences"
    ),
    
    sliderInput(
      "year",
      "Select year",
      min = 2016,
      max = 2025,
      value = 2025,
      sep = ""
    ),
    
    actionButton(
      "button",
      "Click to reset scatterplot to default state, showing offence rates for the whole state"
    )
  ),
  layout_columns(
    card(card_header("Map"), textOutput("map_text"), leafletOutput("map")),
    card(card_header("Scatterplot"), textOutput("scatterplot_text"), plotOutput("scatterplot")),
    col_widths = c(7,5) # Must add to 12
  ),
)

# Define server logic ----
server <- function(input, output) {
  
  # Select data to use to create the map
  mapInputData <- reactive({
    if(input$offence == "All offences"){
      switch(input$map_type,
             "Offence rate" = type_total_polygons[type_total_polygons$Type == 'FV' & type_total_polygons$Year == input$year, ], 
             "Increase in offence rate from 2016 to 2025" = type_total_increase_polygons[type_total_increase_polygons$Type == 'FV', ])
    }
    else{
      switch(input$map_type,
             "Offence rate" = offence_rate_polygons[offence_rate_polygons$Type == 'FV' & offence_rate_polygons$Offence == input$offence & 
                                                      offence_rate_polygons$Year == input$year, ], 
             "Increase in offence rate from 2016 to 2025" = increase_polygons[increase_polygons$Type == 'FV' & 
                                                                                increase_polygons$Offence == input$offence, ])
    }
  })
  
  
  output$map <- renderLeaflet({
    map_data <- st_transform(mapInputData(), crs = '+proj=longlat +datum=WGS84')
    
    # Get bounding box of polygons
    bbox <- st_bbox(map_data)
    
    # Code to fix problem with NA in legend, sourced from https://github.com/rstudio/leaflet/issues/615
    css_fix <- "div.info.legend.leaflet-control br {clear: both;}" # CSS to correct spacing
    html_fix <- htmltools::tags$style(type = "text/css", css_fix)  # Convert CSS to HTML
    
    if(input$map_type == "Offence rate"){
      pal <- colorNumeric(
        palette = c("skyblue", "darkblue"), 
        domain = map_data$Rate,
        na.color = "grey"
      )
      
      leaflet(map_data) %>% 
        addTiles() %>%
        addPolygons(fillColor = ~pal(Rate), 
                    fillOpacity = 0.8,
                    color = "black", 
                    weight = 1,
                    label = paste(
                      "<strong>LGA Name:</strong>", str_to_title(map_data$LGA_NAME),
                      "<br><strong>Offence rate:</strong>", map_data$Rate) %>%
                      lapply(htmltools::HTML),
                    layerId = ~LGA_NAME) %>%
        addLegend("topright", pal = pal, values = ~Rate,
                  title = "Offence Rate",
                  opacity = 1,
                  na.label = "<LC"
        )
    }
    else{
      pal <- colorNumeric(
        palette = c("red", "white", "darkblue"), 
        domain = map_data$Rate,
        na.color = "grey"
      )
      
      leaflet(map_data) %>% 
        addTiles() %>%
        addPolygons(fillColor = ~pal(Rate), 
                    fillOpacity = 0.8,
                    color = "black", 
                    weight = 1,
                    label = paste(
                      "<strong>LGA Name:</strong>", str_to_title(map_data$LGA_NAME),
                      "<br><strong>Increase in offence rate:</strong>", map_data$Rate) %>%
                      lapply(htmltools::HTML),
                    layerId = ~LGA_NAME) %>%
        addLegend("topright", pal = pal, values = ~Rate,
                  title = "Increase in Offence Rate from 2016 to 2025",
                  opacity = 1,
                  na.label = "Data unavailable"
        )
    }
    
  })
  
  # Variable indicating which dataset to use for the scatterplot
  scatterplot_data_indicator <- reactiveValues(location="Victoria (whole state)")
  
  # Change scatterplot data indicator location to be the LGA name when an LGA is clicked on
  observe({
    scatterplot_data_indicator$location <- input$map_shape_click$id
  }) %>% bindEvent(input$map_shape_click)
  
  # Change scatterplot data indicator location to "Victoria (whole state)" when button is clicked
  observe({
    scatterplot_data_indicator$location <- "Victoria (whole state)"
  }) %>% bindEvent(input$button)
  
  # Select data for scatterplot
  scatterplotInputData <- reactive({
    if(scatterplot_data_indicator$location == "Victoria (whole state)" & input$offence == "All offences"){
      whole_state_type_total_data
    }
    else if(scatterplot_data_indicator$location == "Victoria (whole state)" & input$offence != "All offences"){
      subset(whole_state_data, whole_state_data$Offence == input$offence)
    }
    else if(scatterplot_data_indicator$location != "Victoria (whole state)" & input$offence == "All offences"){
      subset(type_total_data, type_total_data$LGA == scatterplot_data_indicator$location)
    }
    else if(scatterplot_data_indicator$location != "Victoria (whole state)" & input$offence != "All offences"){
      subset(offence_rate_data, offence_rate_data$LGA == scatterplot_data_indicator$location & offence_rate_data$Offence == input$offence)
    }

  }) 
  
  # Create scatterplot
  output$scatterplot <- renderPlot({
    ggplot(scatterplotInputData())+
      geom_point(aes(x = Year, y = Rate, colour = Type)) +
      geom_line(aes(x = Year, y = Rate, colour = Type)) +
      labs(x = "Year", 
           y = "Offence Rate",
           colour = "Offence Type") +
      theme(axis.text.x = element_text(angle = 45, hjust = 1)) +
      theme(legend.position = "bottom") +
      scale_x_continuous(breaks = breaks_pretty())
  })
  
  # Display text above each plot, clearly explaining which data is being displayed
  output$map_text <- renderText({
    if(input$map_type == "Offence rate"){
      paste("Currently displaying", tolower(input$map_type), "for", tolower(input$offence), "in", input$year)
    }
    else{
      paste("Currently displaying", tolower(input$map_type), "for", tolower(input$offence))
    }
    
  })
  
  output$scatterplot_text <- renderText({
    paste("Currently displaying offence rate for", tolower(input$offence), "in", str_to_title(scatterplot_data_indicator$location))
  })
  
  
}

# Run the app ----
shinyApp(ui = ui, server = server)