Configuration files for SynthCh10Gen provide the application with datasource and channel setup information for a single generation run. These files are in JSON format and have the following requirements.


## Configuration File Description

|  Key  | Type  | Required | Description  | Default |
| ------------ | ------------ | ------------ | ------------ | ------------ |
| `programName` | `string`  | | Sets the G\PN TMATS attribute | "Synthetic Chapter 10" |
| `outputDirectory` | `string` | Yes | Output location of the generated Chapter 10 data file | |
| `outputFilename` | `string` | | Name of the generated Chapter 10 data file | "synthetic_data_yyyymmdd_hhmmss.ch10" |
| `timeSource` | `string` | | Name of the channel from which to derive simulation time | The first channel defined in the channels array |
| `timeStart` | `string` | | ??? | ??? |
| `channels` | `array` | Yes | Must contain at least one Channel Description object | |

### Channel Description

|  Key  | Type  | Required | Description  | Default |
| ------------ | ------------ | ------------ | ------------ | ------------ |
| id | uint | | Channel ID, sets the R-x\TK1-n TMATS attribute | Starts at 2 and increments by 1 for each unspecified ID |
| name | string | | Channel name, sets the R-x\DSI-n TMATS attribute | Auto generated name |
| type | string | Yes | Channel Type, sets the R-x\CDT-n TMATS attribute | |
| sourceFile | string | Yes | Pathname of a data file used as the source for generation | |
| pollRate | object | | Sets the rate at which the source data is polled | All data is included at the rate it occurs in the source |
| packetRate | | Sets the rate at which packets are published to the output stream | Equal to the poll rate |


### Rate Description

|  Key  | Type  | Required | Description  | Default |
| ------------ | ------------ | ------------ | ------------ | ------------ |
| value | uint | | Numerical rate value | 0, corresponds to unsampled or data-triggered |
| unit | string | | Rate Units | default milliseconds |

### Channel Types
| TMATS Channel Type | Allowed Values (case-insensitive) |
| ---- | ---- |
| PCMIN | pcmin, pcm |
| VIDIN | vidin, video, vid |
| 1553IN | 1553in, 1553, ms1553, mil_std_1553, mil-std-1553 |
| 429IN | 429in, a429, arinc429, arinc-429, arinc_429 |


### Rate Units
| Unit | Allowed Values (case-insensitive) |
| ---- | ---- |
| seconds | seconds, sec, s |
| milliseconds | milliseconds, milli, ms |
| microseconds | microseconds, micro, us |
| rtc (ticks) | rtc |
| nanoseconds | nanoseconds, nano, ns |
| hertz | hertz, hz, frequency |

##  Example Configuration

```
{
	"programName" : "Sample Data Generation",
	"outputDirectory" : "C:/data/output/",
	"outputFilename" : "SampleDataGen.ch10",
	"timeSource" : "PCMin20",
	"timeStart" : ???,
	
	"channels" : [
		{
			"name" : "PCMin20",
			"id" : 20,
			"type" : "pcm",
			"sourceFile" : "C:/data/sources/flight42.csv",
			"pollRate" : {
				"value" : 100,
				"unit" : "hz"
			},
			"packetRate" : {
				"value" : 20,
				"unit" : "hz"
			}
		}
	]
}
```

##  Example Minimal Configuration
```
{	
    "outputDirectory" : "C:/data/output",
	"channels" : [
		{
			"type" : "pcm",
			"sourceFile" : "C:/atac/vsprojects/SyntheticData/Debug/30931-small.csv"
		}
	]
}
```
