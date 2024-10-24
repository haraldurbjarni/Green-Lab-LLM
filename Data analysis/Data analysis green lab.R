setwd("~/Greenlab")

# Load necessary libraries
library(ggplot2)
library(dplyr)
library(tidyr)  # For pivot_longer()
library(reshape2) # For correlation heatmap

# Load dataset 
data <- read.csv("run_table.csv")

# Inspect the data to check column names
str(data)
# View the column names
colnames(data)




# Reshape the data for CPU usage
cpu_data <- data %>%
  select(llm, algorithm, starts_with("cpu_usage")) %>%
  pivot_longer(cols = starts_with("cpu_usage"), 
               names_to = "Core", 
               values_to = "Usage")

# Plot seperate CPU core usage
ggplot(cpu_data, aes(x = algorithm, y = Usage, fill = llm)) +
  geom_bar(stat = "identity", position = "dodge") +
  facet_wrap(~ Core, scales = "free_y") + 
  labs(title = "CPU Usage Per Core Across Algorithms",
       y = "CPU Usage (%)",
       x = "Algorithm") +
  theme_minimal() +
  theme(legend.title = element_blank())




# Calculate total average CPU usage for each algorithm and LLM
avg_cpu_data <- cpu_data %>%
  group_by(llm, algorithm) %>%
  summarise(Average_Usage = mean(Usage, na.rm = TRUE)) %>%
  ungroup()

# Plot the total average CPU usage
ggplot(avg_cpu_data, aes(x = algorithm, y = Average_Usage, fill = llm)) +
  geom_bar(stat = "identity", position = "dodge") +
  labs(title = "Average CPU Usage Across Algorithms",
       y = "Average CPU Usage (%)",
       x = "Algorithm") +
  theme_minimal() +
  theme(legend.title = element_blank())





# Create the plot for total CPU frequency distribution
ggplot(cdata, aes(x = algorithm, y = total_cpu_frequency, fill = llm)) +
  geom_bar(stat = "identity", position = "dodge") +
  labs(title = "Total CPU Frequency Distribution Across Algorithms and LLMs", 
       y = "Total CPU Frequency (MHz)", 
       x = "Algorithm") +
  scale_y_continuous(breaks = seq(0, 8000, by = 2000), limits = c(0, 10000)) +  
  theme_minimal() +
  theme(legend.title = element_blank()) +  
  scale_fill_discrete(name = "LLM") +  
  scale_x_discrete(labels = c("bb" = "BB", "bfa" = "BFA", "swa" = "SWA"))  





# Create the plot for total execution time comparison
ggplot(cdata, aes(x = algorithm, y = run_time, fill = llm)) +
  geom_bar(stat = "identity", position = "dodge") +
  labs(title = "Total Execution Time Comparison by Algorithm and LLM", 
       y = "Total Execution Time (s)", 
       x = "Algorithm") +
  theme_minimal() +
  theme(legend.title = element_blank())  



# Ensure used_memory is in GB
memoryData <- memoryData %>%
  mutate(used_memory_gb = used_memory / (1024^3))  # Convert from bytes to GB

# Plot memory usage (used_memory in GB)
ggplot(data, aes(x = algorithm, y = used_memory_gb, fill = llm)) +
  geom_bar(stat = "identity", position = "dodge") +
  labs(title = "Memory Usage Across Models and Algorithms", 
       y = "Used Memory (GB)", 
       x = "Algorithm") +
  theme_minimal() +
  theme(legend.title = element_blank()) +  
  scale_y_continuous(limits = c(0, 2))  




# Convert used_memory to GB
data <- data %>%
  mutate(used_memory_gb = used_memory / (1024^3))

# Calculate average CPU usage and frequency
data <- data %>%
  mutate(average_cpu_usage = rowMeans(select(data, starts_with("cpu_usage")), na.rm = TRUE),
         average_cpu_freq = rowMeans(select(data, starts_with("cpu_freq")), na.rm = TRUE)) %>%
  mutate(total_energy = dram_energy + pp0_energy + pp1_energy + package_energy)

# Select relevant columns for correlation analysis
correlation_data <- data %>%
  select(total_energy, average_cpu_usage, run_time, used_memory_gb, average_cpu_freq)

# Rename columns
colnames(correlation_data) <- c("Energy Us.", "CPU Us.", "Execution Time", "Memory", "CPU Freq.")

# Calculate correlation matrix
cor_matrix <- cor(correlation_data, use = "complete.obs")

# Convert the correlation matrix to a long format
cor_matrix_melted <- melt(cor_matrix)

# Create the heatmap
ggplot(cor_matrix_melted, aes(Var1, Var2, fill = value)) +
  geom_tile() +
  scale_fill_gradient2(low = "blue", high = "red", mid = "white", 
                       midpoint = 0, limit = c(-1, 1), 
                       name="Correlation") +
  theme_minimal() +
  labs(title = "Correlation Heatmap", 
       x = "Variables", 
       y = "Variables") +
  theme(axis.text.x = element_text(angle = 45, hjust = 1)) +
  geom_text(aes(label = round(value, 2)), color = "black", size = 4) +
  coord_fixed()
