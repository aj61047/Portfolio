library(tidyverse)
library(dplyr)
library(readr)
library(sf)

# Import raw data
criminal_offences <- read.csv("Data/Offences Recorded By Type.csv", header=T)

# Clean criminal_offences dataset

# Remove unnecessary columns
criminal_offences <- subset(criminal_offences, select = -Year.ending)
criminal_offences <- subset(criminal_offences, select = -Police.Service.Area)
criminal_offences <- subset(criminal_offences, select = -PSA.Rate.per.100.000.population)
criminal_offences <- subset(criminal_offences, select = -Offence.Subdivision)
criminal_offences <- subset(criminal_offences, select = -Offence.Count) # I will use rate per 100,000 population for this project, not raw count

# We only want to look at crimes against the person, so keep only rows where column 'Offence.Division' is 'A Crimes against the person'
criminal_offences <- criminal_offences[criminal_offences$Offence.Division == 'A Crimes against the person', ]
# Remove column 'Offence.Division'
criminal_offences <- subset(criminal_offences, select = -Offence.Division)


# Keep only offences that are categorised as FV related or non-FV related
criminal_offences <- subset(criminal_offences, grepl("FV", Offence.Subgroup))

# Convert rates to numeric datatype
criminal_offences$LGA.Rate.per.100.000.population <- as.numeric(gsub(",", "", criminal_offences$LGA.Rate.per.100.000.population))

# Remove offence code from beginning of each value in Offence.Subgroup
criminal_offences$Offence.Subgroup <- sub(".*?\\s", "", criminal_offences$Offence.Subgroup)

# Rename columns
names(criminal_offences)[names(criminal_offences) == 'Local.Government.Area'] <- 'LGA'
names(criminal_offences)[names(criminal_offences) == 'LGA.Rate.per.100.000.population'] <- 'Rate'
names(criminal_offences)[names(criminal_offences) == 'Offence.Subgroup'] <- 'Offence'

# Create 'Type' Column
for (i in 1:nrow(criminal_offences)){
  if (grepl("Non.FV", criminal_offences[i, 'Offence'])){
    criminal_offences[i, 'Type'] <- "Non-FV"
    criminal_offences[i, 'Offence'] <- gsub("Non.FV.", "", criminal_offences[i, 'Offence'])
  }
  else {
    criminal_offences[i, 'Type'] <- "FV"
    criminal_offences[i, 'Offence'] <- gsub("FV.", "", criminal_offences[i, 'Offence'])
  }
}

# Change Colac-Otway to Colac Otway, to match polygon dataset
criminal_offences$LGA <- sub("Colac-Otway", "Colac Otway", criminal_offences$LGA)

# Check number of nulls in each column
print(colSums(is.na(criminal_offences)))

# Check distinct years
unique(criminal_offences$Year)

# Check distinct LGAs
length(unique(criminal_offences$LGA))

# Convert LGAs to uppercase
criminal_offences$LGA <- toupper(criminal_offences$LGA)



full_scaffold <- expand(criminal_offences, Year, LGA, Offence, Type)
criminal_offences <- full_join(criminal_offences, full_scaffold)


# Export final dataset
write.csv(criminal_offences, "Data/project_data.csv", row.names = FALSE)


