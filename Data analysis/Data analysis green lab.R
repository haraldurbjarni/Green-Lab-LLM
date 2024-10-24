setwd("~/Greenlab")

# Load necessary libraries
library(ggplot2)
library(dplyr)
library(tidyr)  # For pivot_longer()
library(reshape2) # For correlation heatmap
library(stats)
library(dplyr)

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




# Convert used_memory to GB
data <- data %>%
  mutate(used_memory_gb = used_memory / (1024^3))

# Plot memory usage (used_memory in GB)
ggplot(data, aes(x = algorithm, y = used_memory_gb, fill = llm)) +
  geom_bar(stat = "identity", position = "dodge") +
  labs(title = "Memory Usage Across Models and Algorithms", 
       y = "Used Memory (GB)", 
       x = "Algorithm") +
  theme_minimal() +
  theme(legend.title = element_blank()) +  
  scale_y_continuous(limits = c(0, 2))  




# Calculate average CPU usage and frequency
data <- data %>%
  mutate(average_cpu_usage = rowMeans(select(data, starts_with("cpu_usage")), na.rm = TRUE) * 4,  # Assuming 4 cores
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



descriptive_stats <- data %>%
  group_by(llm, algorithm) %>%
  summarise(
    Mean_CPU = mean(average_cpu_usage, na.rm = TRUE),
    Mean_Memory = mean(used_memory_gb, na.rm = TRUE),
    Mean_Energy = mean(total_energy, na.rm = TRUE),
    Std_Dev_CPU = sd(average_cpu_usage, na.rm = TRUE)  # Standard deviation of average CPU usage
  ) %>%
  ungroup()
print(descriptive_stats)



# Define a function to check normality
check_normality <- function(data_vector, variable_name) {
  if (length(data_vector) < 3) {
    cat(paste(variable_name, "has insufficient data for normality test.\n"))
    return(NULL)
  }
  
  test_result <- shapiro.test(data_vector)
  p_value <- test_result$p.value
  if (p_value > 0.05) {
    result <- "normally distributed"
  } else {
    result <- "not normally distributed"
  }
  cat(paste(variable_name, "is", result, "with a p-value of", round(p_value, 4)), "\n")
}

# Check normality of relevant variables
check_normality(correlation_data$`Energy Us.`, "Total Energy")
check_normality(correlation_data$`CPU Us.`, "Average CPU Usage")
check_normality(correlation_data$`Execution Time`, "Execution Time")
check_normality(correlation_data$Memory, "Used Memory (GB)")
check_normality(correlation_data$`CPU Freq.`, "Average CPU Frequency")

# Histogram for Total Energy
ggplot(data, aes(x = total_energy, fill = llm)) +
  geom_histogram(bins = 30, position = "dodge", alpha = 0.7) +
  labs(title = "Histogram of Total Energy",
       x = "Total Energy (Joules)", 
       y = "Frequency") +
  facet_grid(llm ~ algorithm) +  # Facet by LLM and algorithm
  theme_minimal()

# Histogram for Average CPU Usage
ggplot(data, aes(x = average_cpu_usage, fill = llm)) +
  geom_histogram(bins = 30, position = "dodge", alpha = 0.7) +
  labs(title = "Histogram of Average CPU Usage",
       x = "Average CPU Usage (%)", 
       y = "Frequency") +
  facet_grid(llm ~ algorithm) +  # Facet by LLM and algorithm
  theme_minimal()

# Histogram for Execution Time
ggplot(data, aes(x = run_time, fill = llm)) +
  geom_histogram(bins = 30, position = "dodge", alpha = 0.7) +
  labs(title = "Histogram of Execution Time",
       x = "Execution Time (s)", 
       y = "Frequency") +
  facet_grid(llm ~ algorithm) +  # Facet by LLM and algorithm
  theme_minimal()

# Histogram for Used Memory
ggplot(data, aes(x = used_memory_gb, fill = llm)) +
  geom_histogram(bins = 30, position = "dodge", alpha = 0.7) +
  labs(title = "Histogram of Used Memory (GB)",
       x = "Used Memory (GB)", 
       y = "Frequency") +
  facet_grid(llm ~ algorithm) +  # Facet by LLM and algorithm
  theme_minimal()

# Histogram for Average CPU Frequency
ggplot(data, aes(x = average_cpu_freq, fill = llm)) +
  geom_histogram(bins = 30, position = "dodge", alpha = 0.7) +
  labs(title = "Histogram of Average CPU Frequency",
       x = "Average CPU Frequency (MHz)", 
       y = "Frequency") +
  facet_grid(llm ~ algorithm) +  # Facet by LLM and algorithm
  theme_minimal()

